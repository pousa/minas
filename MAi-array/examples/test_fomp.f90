    PROGRAM VEC_ADD_DO
      use ctof

      implicit none
      real, dimension(:), pointer :: A, B, C
      INTEGER N, CHUNKSIZE, CHUNK, I
      PARAMETER (N=2000000) 
      PARAMETER (CHUNKSIZE=100) 

      A=>null()
      B=>null()
      C=>null()

      call mai_init()
      call mai_alloc_1D_real(A,sizemem)
      call mai_alloc_1D_real(B,sizemem)	
      call mai_alloc_1D_real(C,sizemem)	
      call mai_cyclic(A) 
      call mai_bind_rows(C) 
      call mai_cyclic(B)  
	
!     Some initializations
      DO I = 1, N
        A(I) = I * 1.0
        B(I) = A(I)
      ENDDO
      CHUNK = CHUNKSIZE
      I = 1 
      C(I) =  0.0 
!$OMP PARALLEL SHARED(A,B,C,CHUNK) PRIVATE(I)

!$OMP DO SCHEDULE(DYNAMIC,CHUNK)
      DO I = 2, N
         C(I) = A(I-1) + B(I)
      ENDDO
!$OMP END DO NOWAIT

!$OMP END PARALLEL

      END

