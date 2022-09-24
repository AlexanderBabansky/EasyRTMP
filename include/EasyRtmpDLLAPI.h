#ifndef EASYRTMPDLLAPI_H
#define EASYRTMPDLLAPI_H

#ifdef EASYRTMP_DLL
#ifdef _WIN32
#ifdef EASYRTMP_DLL_EXPORTS
#define EASYRTMP_DLLAPI __declspec(dllexport)
#else
#define EASYRTMP_DLLAPI __declspec(dllimport)
#endif // EASYRTMP_DLL_EXPORTS
#else
#define EASYRTMP_DLLAPI __attribute__((visibility("default")))
#endif // _WIN32
#else
#define EASYRTMP_DLLAPI
#endif // EASYRTMP_DLL

#endif // EASYRTMPDLLAPI_H
