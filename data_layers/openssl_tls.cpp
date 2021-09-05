#ifdef USE_OPENSSL
#ifdef WIN32
	#include <Windows.h>
	#include <wincrypt.h>
#endif

#include <cassert>
#include <iostream>
#include <openssl/ssl.h>
//#include <openssl/applink.c>
#include <openssl/bio.h>
#include "openssl_tls.h"

using namespace std;

void InitOpenSSL() {
	SSL_library_init();
	SSL_load_error_strings();
}

OpenSSL_TLS::OpenSSL_TLS(ssl_st* ssl, bio_method_st* bio_method, bio_st* bio) :
	m_SSL(ssl), m_BioMethod(bio_method), m_BIO(bio) {

}

void OpenSSL_TLS::send_data(const char* data, size_t data_len)
{
	int sd = SSL_write(m_SSL, data, data_len);
	if (sd != data_len) {
		throw OpenSSLException(OpenSSLError::IO);
	}
}

void OpenSSL_TLS::receive_data(char* out, int len)
{	
	int rd = SSL_read(m_SSL, out, len);
	if (rd != len) {
		throw OpenSSLException(OpenSSLError::IO);
	}
}

void OpenSSL_TLS::destroy()
{
	if (m_SSL) {
		SSL_shutdown(m_SSL);
		SSL_free(m_SSL);
		m_SSL = nullptr;
	}
	if (m_BioMethod) {
		BIO_meth_free(m_BioMethod);
		m_BioMethod = nullptr;
	}
	m_BIO = nullptr;
}

OpenSSL_TLS& OpenSSL_TLS::operator=(OpenSSL_TLS&& o) noexcept
{
	m_SSL = o.m_SSL;
	m_BioMethod = o.m_BioMethod;
	m_BIO = o.m_BIO;
	o.m_SSL = 0;
	o.m_BioMethod = 0;
	o.m_BIO = 0;
	return *this;
}
OpenSSL_TLS::~OpenSSL_TLS()
{
	destroy();
}

void OpenSSL_TLS_Server::cleanup()
{
	if (m_SSL) {
		SSL_shutdown(m_SSL);
		SSL_free(m_SSL);
		m_SSL = nullptr;
	}

	if (meth) {
		BIO_meth_free(meth);
		meth = nullptr;
	}

	bio = nullptr;//memory leak possible

	if (m_Cert) {
		X509_free(m_Cert);
		m_Cert = nullptr;
	}
	if (m_PrivateKey) {
		EVP_PKEY_free(m_PrivateKey);
		m_PrivateKey = nullptr;
	}
	if (ctx) {
		SSL_CTX_free(ctx);
		ctx = nullptr;
	}
}

OpenSSL_TLS_Server::~OpenSSL_TLS_Server()
{
	cleanup();
}

int bwrite(BIO* bio, const char* data, int data_len) {
	DataLayer* user_ptr = reinterpret_cast<DataLayer*>(BIO_get_data(bio));
	try {
		user_ptr->send_data(data, data_len);
	}
	catch (exception& e) {
		return -1;
	}
	return data_len;
}

int bread(BIO* bio, char* data, int data_len) {
	DataLayer* user_ptr = reinterpret_cast<DataLayer*>(BIO_get_data(bio));
	try {
		user_ptr->receive_data(data, data_len);
	}
	catch (exception& e) {
		return -1;
	}
	return data_len;
}

int bio_create(BIO* b) {
	return 1;
}

long on_ctrl(BIO* b, int cmd, long l, void* p) {
	return 1;
}

int verify(int preverify_ok, X509_STORE_CTX* x509_ctx) {
	auto err = X509_STORE_CTX_get_error(x509_ctx);
	err = X509_V_OK;
	X509_STORE_CTX_set_error(x509_ctx, err);
	auto err_cert = X509_STORE_CTX_get_current_cert(x509_ctx);
	return 1;
}


inline void InitBIO(BIO_METHOD** meth, BIO** bio, DataLayer* dl) {
	*meth = BIO_meth_new(BIO_get_new_index() | BIO_TYPE_SOURCE_SINK, "Custom");
	if (!*meth) {
		throw OpenSSLException(OpenSSLError::ALLOC);
	}
	BIO_meth_set_write(*meth, &bwrite);
	BIO_meth_set_ctrl(*meth, &on_ctrl);
	BIO_meth_set_read(*meth, &bread);
	BIO_meth_set_create(*meth, &bio_create);
	*bio = BIO_new(*meth);
	if (!*bio) {
		throw OpenSSLException(OpenSSLError::ALLOC);//memory leak possible
	}
	BIO_set_init(*bio, 1);
	BIO_set_data(*bio, dl);
}

inline void InitCTX(const SSL_METHOD* method, SSL_CTX** ctx) {
	*ctx = SSL_CTX_new(method);
	if (!*ctx) {
		throw OpenSSLException(OpenSSLError::ALLOC);
	}
	const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3;
	SSL_CTX_set_options(*ctx, flags);
	SSL_CTX_set_min_proto_version(*ctx, TLS1_2_VERSION);
}

inline void InitSSL(SSL_CTX* ctx, SSL** m_SSL) {
	*m_SSL = SSL_new(ctx);
	if (!*m_SSL) {
		throw OpenSSLException(OpenSSLError::ALLOC);
	}
}

inline void LoadServerCert(const char* certificate_filepath, const char* private_key_filepath, X509** m_Cert, EVP_PKEY** m_PrivateKey) {
#ifdef WIN32
	wchar_t certificate_filepath_w[500];
	mbstowcs(certificate_filepath_w, certificate_filepath, 500);
	FILE* cert_file = 0;
	if (_wfopen_s(&cert_file, certificate_filepath_w, L"r")) {
		throw OpenSSLException(OpenSSLError::FILE_OPEN);
	}
	*m_Cert = PEM_read_X509(cert_file, 0, 0, 0);
	fclose(cert_file);
	if (!*m_Cert) {
		throw OpenSSLException(OpenSSLError::PEM_READ);
	}

	wchar_t private_key_filepath_w[500];
	mbstowcs(private_key_filepath_w, private_key_filepath, 500);
	FILE* key_file = 0;
	if (_wfopen_s(&key_file, private_key_filepath_w, L"r")) {
		throw OpenSSLException(OpenSSLError::FILE_OPEN);
	}
	*m_PrivateKey = PEM_read_PrivateKey(key_file, 0, 0, 0);
	fclose(key_file);
	if (!*m_PrivateKey) {
		throw OpenSSLException(OpenSSLError::PEM_READ);
	}
#endif // WIN32
}

shared_ptr<OpenSSL_TLS> OpenSSL_TLS_Server::handshake(DataLayer* lower_l, const char* certificate_filepath, const char* private_key_filepath)
{
	assert(lower_l && certificate_filepath && private_key_filepath);
	cleanup();
	m_LowerLevel = lower_l;
	SSL_library_init();
	SSL_load_error_strings();	
	InitCTX(SSLv23_server_method(),&ctx);
	SSL_CTX_set_ecdh_auto(ctx, 1);	
	LoadServerCert(certificate_filepath,private_key_filepath,&m_Cert,&m_PrivateKey);	
	if (SSL_CTX_use_certificate(ctx, m_Cert) <= 0) {
		throw OpenSSLException(OpenSSLError::INVALID_CERT);
	}
	if (SSL_CTX_use_PrivateKey(ctx, m_PrivateKey) <= 0) {
		throw OpenSSLException(OpenSSLError::INVALID_PRIVATE_KEY);
	}		
	InitSSL(ctx,&m_SSL);
	InitBIO(&meth,&bio,m_LowerLevel);
	SSL_set_bio(m_SSL, bio, bio);
	if (SSL_accept(m_SSL) <= 0) {
		throw OpenSSLException(OpenSSLError::HANDSHAKE);
	}
	BIO_METHOD* meth_copy = meth;
	BIO* bio_copy = bio;
	SSL* ssl_copy = m_SSL;
	meth = nullptr;
	bio = nullptr;
	m_SSL = nullptr;
	return make_shared<OpenSSL_TLS>(ssl_copy,meth_copy,bio_copy);
}


void OpenSSL_TLS_Client::LoadTrustedCerts()
{
#ifdef WIN32
	HCERTSTORE hCertStore = nullptr;
	PCCERT_CONTEXT pCertContext = NULL;
	hCertStore = CertOpenStore(
		CERT_STORE_PROV_SYSTEM,
		0,
		NULL,
		CERT_SYSTEM_STORE_CURRENT_USER,
		L"Root"
	);
	if (!hCertStore) {
		throw OpenSSLException(OpenSSLError::SYSTEM_CERT_STORE);
	}
	auto ossl_cert_store = SSL_CTX_get_cert_store(ctx);
	while (pCertContext = CertEnumCertificatesInStore(hCertStore, pCertContext)) {
		if (pCertContext->dwCertEncodingType != X509_ASN_ENCODING)continue;
		BIO* mem = BIO_new_mem_buf(pCertContext->pbCertEncoded, pCertContext->cbCertEncoded);
		X509* x = d2i_X509_bio(mem, NULL);
		BIO_free(mem);
		X509_STORE_add_cert(ossl_cert_store, x);
		X509_free(x);
	}
	CertFreeCertificateContext(pCertContext);
	CertCloseStore(hCertStore, 0);
#endif // WIN32

}

void OpenSSL_TLS_Client::cleanup()
{
	if (m_SSL) {
		SSL_shutdown(m_SSL);
		SSL_free(m_SSL);
		m_SSL = nullptr;
	}
	bio = nullptr;//memory leak possible
	if (meth) {
		BIO_meth_free(meth);
		meth = nullptr;
	}	
	if (ctx) {
		SSL_CTX_free(ctx);
		ctx = nullptr;
	}
}

std::shared_ptr<OpenSSL_TLS> OpenSSL_TLS_Client::handshake(DataLayer* lower_l,const char* host_name)
{
	m_LowerLevel = lower_l;	
	InitCTX(SSLv23_client_method(), &ctx);	
	LoadTrustedCerts();
	SSL_CTX_set_session_cache_mode(ctx, SSL_SESS_CACHE_CLIENT | SSL_SESS_CACHE_NO_INTERNAL_STORE);
	SSL_CTX_set_verify(ctx,SSL_VERIFY_PEER,0);

	InitSSL(ctx, &m_SSL);
	auto param = SSL_get0_param(m_SSL);
	X509_VERIFY_PARAM_set1_host(param, host_name, strlen(host_name));

	InitBIO(&meth, &bio, m_LowerLevel);
	SSL_set_bio(m_SSL, bio, bio);
	if (SSL_connect(m_SSL) < 0) {
		throw OpenSSLException(OpenSSLError::HANDSHAKE);
	}		

	BIO_METHOD* meth_copy = meth;
	BIO* bio_copy = bio;
	SSL* ssl_copy = m_SSL;
	meth = nullptr;
	bio = nullptr;
	m_SSL = nullptr;
	return make_shared<OpenSSL_TLS>(ssl_copy, meth_copy, bio_copy);
}

OpenSSL_TLS_Client::OpenSSL_TLS_Client()
{
	InitOpenSSL();
}

OpenSSL_TLS_Client::~OpenSSL_TLS_Client()
{
	cleanup();
}
#endif