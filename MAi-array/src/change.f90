!/*=====================================================================================
!  MAi is a interface to manage memory policy in NUMA architectures
!
!  (C) Copyright 2010 INRIA 
!  Projet: MESCAL / ANR NUMASIS
!
!  Author: Christiane Pousa Ribeiro
!
!  The author may be reached at pousa@imag.fr
!*====================================================================================*/

 subroutine c_1dreal_f(cref,carray,nx)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	real, dimension(nx),target :: carray
	real, dimension(:),pointer :: cref
	cref => carray
      end subroutine c_1dreal_f

 subroutine c_1dlreal_f(cref,carray,nx)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	real*8, dimension(nx),target :: carray
	real*8, dimension(:),pointer :: cref
	cref => carray
      end subroutine c_1dlreal_f

 subroutine c_1dint_f(cref,carray,nx)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer, dimension(nx),target :: carray
	integer, dimension(:),pointer :: cref
	cref => carray
      end subroutine c_1dint_f

 subroutine c_1dbyte_f(cref,carray,nx)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	byte, dimension(nx),target :: carray
	byte, dimension(:),pointer :: cref
	cref => carray
      end subroutine c_1dbyte_f

 subroutine c_1dcomplex_f(cref,carray,nx)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	complex, dimension(nx),target :: carray
	complex, dimension(:),pointer :: cref
	cref => carray
      end subroutine c_1dcomplex_f



 subroutine c_2dlreal_f(cref,carray,nx,ny)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	real*8, dimension(nx,ny),target :: carray
	real*8, dimension(:,:),pointer :: cref
	cref => carray
      end subroutine c_2dlreal_f

 subroutine c_2dint_f(cref,carray,nx,ny)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer, dimension(nx,ny),target :: carray
	integer, dimension(:,:),pointer :: cref
	cref => carray
      end subroutine c_2dint_f

 subroutine c_2dbyte_f(cref,carray,nx,ny)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	byte, dimension(nx,ny),target :: carray
	byte, dimension(:,:),pointer :: cref
	cref => carray
      end subroutine c_2dbyte_f

 subroutine c_2dcomplex_f(cref,carray,nx,ny)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	complex, dimension(nx,ny),target :: carray
	complex, dimension(:,:),pointer :: cref
	cref => carray
      end subroutine c_2dcomplex_f

 subroutine c_3dreal_f(cref,carray,nx,ny,nz)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	real, dimension(nx,ny,nz),target :: carray
	real, dimension(:,:,:),pointer :: cref
	cref => carray
      end subroutine c_3dreal_f

 subroutine c_3dlreal_f(cref,carray,nx,ny,nz)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	real*8, dimension(0:nx,0:ny,0:nz),target :: carray
	real*8, dimension(:,:,:),pointer :: cref
	cref => carray
      end subroutine c_3dlreal_f

 subroutine c_3dint_f(cref,carray,nx,ny,nz)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer, dimension(nx,ny,nz),target :: carray
	integer, dimension(:,:,:),pointer :: cref
	cref => carray
      end subroutine c_3dint_f

 subroutine c_3dbyte_f(cref,carray,nx,ny,nz)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	byte, dimension(nx,ny,nz),target :: carray
	byte, dimension(:,:,:),pointer :: cref
	cref => carray
      end subroutine c_3dbyte_f

 subroutine c_3dcomplex_f(cref,carray,nx,ny,nz)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	complex, dimension(nx,ny,nz),target :: carray
	complex, dimension(:,:,:),pointer :: cref
	cref => carray
      end subroutine c_3dcomplex_f

 subroutine c_4dreal_f(cref,carray,nx,ny,nz,nk)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	real, dimension(nx,ny,nz,nk),target :: carray
	real, dimension(:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_4dreal_f

 subroutine c_4dlreal_f(cref,carray,nx,ny,nz,nk)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	real*8, dimension(nx,ny,nz,nk),target :: carray
	real*8, dimension(:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_4dlreal_f

 subroutine c_4dint_f(cref,carray,nx,ny,nz,nk)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	integer, dimension(nx,ny,nz,nk),target :: carray
	integer, dimension(:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_4dint_f

 subroutine c_4dbyte_f(cref,carray,nx,ny,nz,nk)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	byte, dimension(nx,ny,nz,nk),target :: carray
	byte, dimension(:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_4dbyte_f

 subroutine c_4dcomplex_f(cref,carray,nx,ny,nz,nk)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	complex, dimension(nx,ny,nz,nk),target :: carray
	complex, dimension(:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_4dcomplex_f

 subroutine c_5dreal_f(cref,carray,nx,ny,nz,nk,nl)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	integer :: nl
	real, dimension(nx,ny,nz,nk,nl),target :: carray
	real, dimension(:,:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_5dreal_f

 subroutine c_5dlreal_f(cref,carray,nx,ny,nz,nk,nl)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	integer :: nl
	real*8, dimension(nx,ny,nz,nk,nl),target :: carray
	real*8, dimension(:,:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_5dlreal_f

 subroutine c_5dint_f(cref,carray,nx,ny,nz,nk,nl)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	integer :: nl
	integer, dimension(nx,ny,nz,nk,nl),target :: carray
	integer, dimension(:,:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_5dint_f

 subroutine c_5dbyte_f(cref,carray,nx,ny,nz,nk,nl)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	integer :: nl
	byte, dimension(nx,ny,nz,nk,nl),target :: carray
	byte, dimension(:,:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_5dbyte_f

 subroutine c_5dcomplex_f(cref,carray,nx,ny,nz,nk,nl)
     	! set reference to data in Fortran object
     	implicit none
	integer :: nx
	integer :: ny
	integer :: nz
	integer :: nk
	integer :: nl
	complex, dimension(nx,ny,nz,nk,nl),target :: carray
	complex, dimension(:,:,:,:,:),pointer :: cref
	cref => carray
      end subroutine c_5dcomplex_f
