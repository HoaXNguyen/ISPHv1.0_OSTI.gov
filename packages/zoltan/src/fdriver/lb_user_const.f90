module lb_user_const
use zoltan_types
implicit none

!  This version of lb_user_const.f90 is a modification of the example
!  lb_user_const_int.f90, modified for use with fdriver by adding the
!  definition of type(ELEM_INFO) and defining LB_User_Data_1.

!  This file contains the user-defined data types and comparison functions
!  for global and local IDs used by the application and the
!  load-balancing library.  Application developers should modify these
!  data types to match those of identifiers used in their applications.
!
!  In this example, LB_GID and LB_LID are both INTEGER(KIND=LB_INT).
!  In this case, the types are not defined, and the operators are
!  performed by C macros in the file lb_user_const_int.h.
!
!  LB_GID are the unique global identifiers for objects in the application.  
!  The LB_GID are used as identifiers within the load-balancing routines
!  as well.  Thus, functions defining methods to compare global identifiers 
!  must be provided.
!
!  LB_LID are local identifiers that are not used by the load-balancing
!  routines.  They are stored with objects in the load-balancing routines,
!  however, and are passed to the application query functions.  An 
!  application can provide any values it wants for local identifiers, and
!  can use them to make access of object information in the query routines
!  more efficient.

!  LB_GID and LB_LID data type definitions.
!  For this example, global IDs (LB_GID) and local IDs (LB_LID) are both
!  integers, so these are just dummy definitions to satisfy the linker.

type LB_GID
   integer(LB_INT) :: id
end type LB_GID

type LB_LID
   integer(LB_INT) :: id
end type LB_LID

! User defined data types for passing data to the query functions.  These can
! be used any way you want, but one suggestion is to use them as "wrapper"
! types for your own user defined types, e.g.
! type LB_User_Data_1 
!    type(your_type), pointer :: ptr
! end type
! Exactly four data types must be defined, but you don't have to use them.

!/*
! * Structure used to describe an element. Each processor will
! * allocate an array of these structures.
!   Moved here from dr_consts.f90 so that User_Data can use it
! */
type ELEM_INFO
  integer(LB_INT) :: border   ! set to 1 if this element is a border element
  integer(LB_INT) :: globalID ! Global ID of this element, the local ID is the
                             ! position in the array of elements
  integer(LB_INT) :: elem_blk ! element block number which this element is in
  real(LB_FLOAT)  :: cpu_wgt  ! the computational weight associated with the elem
  real(LB_FLOAT)  :: mem_wgt  ! the memory weight associated with the elem
  real(LB_FLOAT), pointer ::  coord(:,:) ! array for the coordinates of the
                                        ! element. For Nemesis meshes, nodal
                                        ! coordinates are stored; for Chaco
                                        ! graphs with geometry, one set of
                                        ! coords is stored.
  integer(LB_INT), pointer :: connect(:) ! list of nodes that make up this
                                        ! element, the node numbers in this
                                        ! list are global and not local
  integer(LB_INT), pointer :: adj(:)  ! list of adjacent elements .
                         ! For Nemesis input, the list is ordered by
                         ! side number, to encode side-number info needed to
                         ! rebuild communication maps.  Value -1 represents
                         ! sides with no neighboring element (e.g., along mesh
                         ! boundaries).  Chaco doesn't have "sides," so the
                         ! ordering is irrelevent for Chaco input.
  integer(LB_INT), pointer :: adj_proc(:) ! list of processors for adjacent
                                         ! elements
  real(LB_FLOAT), pointer :: edge_wgt(:)  ! edge weights for adjacent elements
  integer(LB_INT) :: nadj    ! number of entries in adj
  integer(LB_INT) :: adj_len ! allocated length of adj/adj_proc/edge_wgt arrays
end type

type LB_User_Data_1
   type(ELEM_INFO), pointer :: ptr(:)
end type LB_User_Data_1

type LB_User_Data_2
   integer :: dummy
end type LB_User_Data_2

type LB_User_Data_3
   integer :: dummy
end type LB_User_Data_3

type LB_User_Data_4
   integer :: dummy
end type LB_User_Data_4

contains

!  Subroutines to copy LB_GIDs and LB_LIDs.
!  These subroutines are used by the load-balancing routines to copy LB_GID and
!  LB_LID values to new storage locations.
!  In this example, these operations are performed by C macros in
!  lb_user_const_int.h, so these are just dummy routines that will never
!  be called.

subroutine LB_SET_GID(a,b)
integer(LB_INT), intent(out) :: a
integer(LB_INT), intent(in) :: b
a = b
end subroutine LB_SET_GID

subroutine LB_SET_LID(a,b)
integer(LB_INT), intent(out) :: a
integer(LB_INT), intent(in) :: b
a = b
end subroutine LB_SET_LID

!  Functions to compare LB_GIDs.
!  Functions must be provided to test whether two LB_GIDs are equal (EQ),
!  not equal (NE), less than (LT), less than or equal (LE), 
!  greater than (GT), and greater than or equal (GE).
!  The function must return the value 1 if the comparison yields .true. and
!  0 if the comparison yeilds .false.
!  Comparison functions are not needed for LB_LIDs as LB_LIDs are not used
!  within the load-balancing routines.
!  In this example, these operations are performed by C macros in
!  lb_user_const_int.h, so these are just dummy routines that will never
!  be called.

function LB_EQ_GID(a,b)
integer(LB_INT) :: LB_EQ_GID
integer(LB_INT), intent(in) :: a,b
if (a == b) then
   LB_EQ_GID = 1_LB_INT
else
   LB_EQ_GID = 0_LB_INT
endif
end function LB_EQ_GID

function LB_NE_GID(a,b)
integer(LB_INT) :: LB_NE_GID
integer(LB_INT), intent(in) :: a,b
if (a /= b) then
   LB_NE_GID = 1_LB_INT
else
   LB_NE_GID = 0_LB_INT
endif
end function LB_NE_GID

function LB_LT_GID(a,b)
integer(LB_INT) :: LB_LT_GID
integer(LB_INT), intent(in) :: a,b
if (a < b) then
   LB_LT_GID = 1_LB_INT
else
   LB_LT_GID = 0_LB_INT
endif
end function LB_LT_GID

function LB_LE_GID(a,b)
integer(LB_INT) :: LB_LE_GID
integer(LB_INT), intent(in) :: a,b
if (a <= b) then
   LB_LE_GID = 1_LB_INT
else
   LB_LE_GID = 0_LB_INT
endif
end function LB_LE_GID

function LB_GT_GID(a,b)
integer(LB_INT) :: LB_GT_GID
integer(LB_INT), intent(in) :: a,b
if (a > b) then
   LB_GT_GID = 1_LB_INT
else
   LB_GT_GID = 0_LB_INT
endif
end function LB_GT_GID

function LB_GE_GID(a,b)
integer(LB_INT) :: LB_GE_GID
integer(LB_INT), intent(in) :: a,b
if (a >= b) then
   LB_GE_GID = 1_LB_INT
else
   LB_GE_GID = 0_LB_INT
endif
end function LB_GE_GID

end module lb_user_const
