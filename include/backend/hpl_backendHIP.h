#pragma once

#include <hip/hip_runtime.h>
#if defined(HPLHIP_USE_ROCRAND)
#include <rocrand.h>
#endif
#include <rocblas.h>

#include <cstdio>
#include <cstdlib>
#include <map>
#include <string>

extern "C" {
#include "hpl_pmatgen.h"
#include "hpl_panel.h"
}


#define HIP_CHECK_ERROR(error)                        \
    if(error != hipSuccess)                           \
    {                                                 \
        fprintf(stderr,                               \
                "hip error: '%s'(%d) at %s:%d\n",     \
                hipGetErrorString(error),             \
                error,                                \
                __FILE__,                             \
                __LINE__);                            \
        exit(EXIT_FAILURE);                           \
    }

#define ROCRAND_CHECK_STATUS(status)                  \
  {                                                   \
    rocrand_status _status = status;                  \
    if(_status != ROCRAND_STATUS_SUCCESS) {           \
        fprintf(stderr,                               \
                "rocRAND error: (%d) at %s:%d\n",     \
                _status,                              \
                __FILE__,                             \
                __LINE__);                            \
        exit(EXIT_FAILURE);                           \
    }                                                 \
  }

#define ROCBLAS_CHECK_STATUS(status)                  \
    if(status != rocblas_status_success)              \
    {                                                 \
        fprintf(stderr,                               \
                "rocBLAS error: '%s'(%d) at %s:%d\n", \
                rocblas_status_to_string(status),     \
                status,                               \
                __FILE__,                             \
                __LINE__);                            \
        exit(EXIT_FAILURE);                           \
    }


#define GPUError(...) GPUInfo(__VA_ARGS__)

//#define HPL_PRINT_INFO
#ifdef HPL_PRINT_INFO
#define GPUInfo(msg, ...)                             \
{                                                     \
  printf("INFO\t %-15s\t" msg "\n", ##__VA_ARGS__);   \
}
#else
#define GPUInfo(msg, ...)
#endif



namespace HIP {
enum HPL_EVENT {PANEL_COPY, PANEL_UPDATE};

    void init(size_t);
    void release();

    void malloc(void**, size_t);
    void free(void **);
    void panel_new(HPL_T_grid *, HPL_T_palg *, const int, const int, const int, HPL_T_pmat *,
                   const int, const int, const int, HPL_T_panel **);
    void panel_init(HPL_T_grid *, HPL_T_palg *, const int,
                    const int, const int, HPL_T_pmat *,
                    const int, const int, const int, HPL_T_panel *);
    void panel_send_to_host(HPL_T_panel *);
    void panel_send_to_device(HPL_T_panel *);
    int panel_free(HPL_T_panel *);
    int panel_disp(HPL_T_panel**);

    void matgen(const HPL_T_grid *, const int, const int,
                 const int, double *, const int,
                 const int);
    void event_record(enum HPL_EVENT);
/*
*  ----------------------------------------------------------------------
*  - BLAS ---------------------------------------------------------------
*  ----------------------------------------------------------------------
*/ 

    int  idamax(const int, const double *, const int);
    void daxpy(const int, const double, const double *, const int, double *, 
                    const int);
    void dscal(const int, const double, double *, const int);
    void dswap(const int, double *, const int, double *, const int);

    void dger( const enum HPL_ORDER, const int, const int, const double, const double *,
                  const int, double *, const int, double *, const int);




    void trsm( const enum HPL_ORDER, const enum HPL_SIDE, 
                const enum HPL_UPLO, const enum HPL_TRANS, 
                const enum HPL_DIAG, const int, const int, 
                const double, const double *, const int, double *, const int);
    void trsv(const enum HPL_ORDER, const enum HPL_UPLO,
                const enum HPL_TRANS, const enum HPL_DIAG,
                const int, const double *, const int,
                double *, const int);                  
    void dgemm(const enum HPL_ORDER, const enum HPL_TRANS, 
                const enum HPL_TRANS, const int, const int, const int, 
                const double, const double *, const int, 
                const double *, const int, const double, double *, 
                const int);
    void dgemv(const enum HPL_ORDER, const enum HPL_TRANS, const int, const int,
                const double, const double *, const int, const double *, const int,
                const double, double *, const int);
    void copy(const int, const double *, const int, double *, const int);                
    void acpy(const int, const int, const double *, const int, double *, const int);                
    void atcpy(const int, const int, const double *, const int,
                double *, const int);  

    void move_data(double *, const double *, const size_t, const int);

    void dlaswp00N(const int, const int, double *, const int, const int *);
    void gPrintMat(const int, const int, const int, const double*);
    // BLAS members
    namespace {
      rocblas_handle _handle;
      static char     host_name[MPI_MAX_PROCESSOR_NAME];
      hipStream_t computeStream, dataStream;
      hipEvent_t panelUpdate;
      hipEvent_t panelCopy;
      hipEvent_t dlaswpStart, dlaswpStop;
      hipEvent_t dtrsmStart, dtrsmStop;
      hipEvent_t dgemmStart, dgemmStop;
      std::map<int, const char*> _memcpyKind;
    }
}
