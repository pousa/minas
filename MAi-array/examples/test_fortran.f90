program mem
      use ctof

      implicit none
      real, dimension(:,:), pointer :: w1
      integer :: i, j, sizemem
      w1=>null()
      write (unit=*, fmt="(a)") "How big?"
      read (unit=*, fmt="(i10)") sizemem
      call mai_init()
      call mai_alloc_2d_real(w1,sizemem,sizemem)
      call mai_skew_mapp(w1) 

      print *, w1 (sizemem,sizemem)

      do i = 1, (sizemem)
         j = i
         w1 (i,j) = 5.2
      end do
      print *, w1 (sizemem,sizemem)
end program 



