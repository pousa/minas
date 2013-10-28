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

module ctof
      public

      interface
      subroutine mai_alloc_1d_real(carray,nx)
      implicit none
      integer :: nx
      real, dimension(:), pointer :: carray
      end subroutine mai_alloc_1d_real	
      end interface

      interface
      subroutine mai_alloc_1d_lreal(carray,nx)
      implicit none
      integer :: nx
      real*8, dimension(:), pointer :: carray
      end subroutine mai_alloc_1d_lreal	
      end interface

      interface
      subroutine mai_alloc_1d_int(carray,nx)
      implicit none
      integer :: nx
      integer, dimension(:), pointer :: carray
      end subroutine mai_alloc_1d_int	
      end interface

      interface
      subroutine mai_alloc_1d_byte(carray,nx)
      implicit none
      integer :: nx
      byte, dimension(:), pointer :: carray
      end subroutine mai_alloc_1d_byte	
      end interface

      interface
      subroutine mai_alloc_1d_complex(carray,nx)
      implicit none
      integer :: nx
      complex, dimension(:), pointer :: carray
      end subroutine mai_alloc_1d_complex	
      end interface

      interface
      subroutine mai_alloc_2d_real(carray,nx,ny)
      implicit none
      integer :: nx
      integer :: ny
      real, dimension(:,:), pointer :: carray
      end subroutine mai_alloc_2d_real	
      end interface

      interface
      subroutine mai_alloc_2d_lreal(carray,nx,ny)
      implicit none
      integer :: nx
      integer :: ny
      real*8, dimension(:,:), pointer :: carray
      end subroutine mai_alloc_2d_lreal	
      end interface

      interface
      subroutine mai_alloc_2d_byte(carray,nx,ny)
      implicit none
      integer :: nx
      integer :: ny
      byte, dimension(:,:), pointer :: carray
      end subroutine mai_alloc_2d_byte	
      end interface

      interface
      subroutine mai_alloc_2d_int(carray,nx,ny)
      implicit none
      integer :: nx
      integer :: ny
      integer, dimension(:,:), pointer :: carray
      end subroutine mai_alloc_2d_int	
      end interface

      interface
      subroutine mai_alloc_2d_complex(carray,nx,ny)
      implicit none
      integer :: nx
      integer :: ny
      complex, dimension(:,:), pointer :: carray
      end subroutine mai_alloc_2d_complex	
      end interface

      interface
      subroutine mai_alloc_3d_real(carray,nx,ny,nz)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      real, dimension(:,:,:), pointer :: carray
      end subroutine mai_alloc_3d_real	
      end interface

      interface
      subroutine mai_alloc_3d_lreal(carray,nx,ny,nz)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      real*8, dimension(:,:,:), pointer :: carray
      end subroutine mai_alloc_3d_lreal	
      end interface

      interface
      subroutine mai_alloc_3d_byte(carray,nx,ny,nz)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      byte, dimension(:,:,:), pointer :: carray
      end subroutine mai_alloc_3d_byte	
      end interface

      interface
      subroutine mai_alloc_3d_int(carray,nx,ny,nz)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer, dimension(:,:,:), pointer :: carray
      end subroutine mai_alloc_3d_int	
      end interface

      interface
      subroutine mai_alloc_3d_complex(carray,nx,ny,nz)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      complex, dimension(:,:,:), pointer :: carray
      end subroutine mai_alloc_3d_complex	
      end interface

      interface
      subroutine mai_alloc_4d_real(carray,nx,ny,nz,nk)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      real, dimension(:,:,:,:), pointer :: carray
      end subroutine mai_alloc_4d_real	
      end interface

      interface
      subroutine mai_alloc_4d_lreal(carray,nx,ny,nz,nk)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      real*8, dimension(:,:,:,:), pointer :: carray
      end subroutine mai_alloc_4d_lreal	
      end interface

      interface
      subroutine mai_alloc_4d_byte(carray,nx,ny,nz,nk)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      byte, dimension(:,:,:,:), pointer :: carray
      end subroutine mai_alloc_4d_byte	
      end interface

      interface
      subroutine mai_alloc_4d_int(carray,nx,ny,nz,nk)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      integer, dimension(:,:,:,:), pointer :: carray
      end subroutine mai_alloc_4d_int	
      end interface

      interface
      subroutine mai_alloc_4d_complex(carray,nx,ny,nz,nk)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      complex, dimension(:,:,:,:), pointer :: carray
      end subroutine mai_alloc_4d_complex	
      end interface

    interface
      subroutine mai_alloc_5d_real(carray,nx,ny,nz,nk,nl)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      integer :: nl
      real, dimension(:,:,:,:,:), pointer :: carray
      end subroutine mai_alloc_5d_real	
      end interface

      interface
      subroutine mai_alloc_5d_lreal(carray,nx,ny,nz,nk,nl)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      integer :: nl
      real*8, dimension(:,:,:,:,:), pointer :: carray
      end subroutine mai_alloc_5d_lreal	
      end interface

      interface
      subroutine mai_alloc_5d_byte(carray,nx,ny,nz,nk,nl)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      integer :: nl
      byte, dimension(:,:,:,:,:), pointer :: carray
      end subroutine mai_alloc_5d_byte	
      end interface

      interface
      subroutine mai_alloc_5d_int(carray,nx,ny,nz,nk,nl)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      integer :: nl
      integer, dimension(:,:,:,:,:), pointer :: carray
      end subroutine mai_alloc_5d_int	
      end interface

      interface
      subroutine mai_alloc_5d_complex(carray,nx,ny,nz,nk,nl)
      implicit none
      integer :: nx
      integer :: ny
      integer :: nz
      integer :: nk
      integer :: nl
      complex, dimension(:,:,:,:,:), pointer :: carray
      end subroutine mai_alloc_5d_complex
      end interface

end module ctof

