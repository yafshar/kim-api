!*******************************************************************************
!**
!**  PROGRAM TEST_NAME_STR
!**
!**  KIM compliant program to find (using the Golden section search algorithm)
!**  the minimum energy of one atom in a periodic FCC crystal of SPECIES_NAME_STR as a 
!**  function of lattice spacing.
!**
!**  Works with the following NBC scenarios:
!**        MI-OPBC-H
!**        MI-OPBC-F
!**
!**  Authors: Valeriu Smirichinski, Ryan S. Elliott, Ellad B. Tadmor
!**
!**  Copyright 2011 Ellad B. Tadmor, Ryan S. Elliott, and James P. Sethna
!**  All rights reserved.
!**
!*******************************************************************************


!-------------------------------------------------------------------------------
!
! Main program
!
!-------------------------------------------------------------------------------
program TEST_NAME_STR
  use KIMservice
  implicit none

  integer,                  external  :: get_neigh_no_Rij
  double precision,         parameter :: TOL         = 1.0d-8
  double precision,         parameter :: Golden      = (1.d0 + sqrt(5.d0))/2.d0
  double precision,         parameter :: FCCspacing  = FCC_SPACING_STR
  double precision,         parameter :: MinSpacing  = 0.800d0*FCCspacing
  double precision,         parameter :: MaxSpacing  = 1.200d0*FCCspacing
  double precision,         parameter :: SpacingIncr = 0.025d0*FCCspacing
  integer,                  parameter :: DIM               = 3
  integer,                  parameter :: ATypes            = 1
  integer(kind=kim_intptr), parameter :: SizeOne           = 1
  logical                             :: halfflag
  integer                             :: middleDum

  ! neighbor list
  integer, allocatable          :: neighborList(:,:)

  !
  ! KIM variables
  !
  character*80              :: testname     = "TEST_NAME_STR"
  character*80              :: modelname
  character*64 :: NBC_Method; pointer(pNBC_Method,NBC_Method)
  integer :: nbc  ! 0- MI-OPBC-H, 1- MI-OPBC-F
  integer(kind=kim_intptr)  :: pkim
  integer                   :: ier
  integer(kind=8) numberOfAtoms; pointer(pnAtoms,numberOfAtoms)
  integer numberAtomTypes;       pointer(pnAtomTypes,numberAtomTypes)
  integer atomTypesdum(1);       pointer(patomTypesdum,atomTypesdum)

  real*8 cutoff;           pointer(pcutoff,cutoff)
  real*8 energy;           pointer(penergy,energy)
  real*8 coordum(DIM,1);   pointer(pcoor,coordum)
  real*8 boxlength(DIM);   pointer(pboxlength,boxlength)
  integer CellsPerSide
  integer N4
  real*8, pointer  :: coords(:,:)
  integer, pointer :: atomTypes(:)
  integer(kind=kim_intptr) :: N
  double precision :: Spacings(4)
  double precision :: Energies(4)


  ! Get KIM Model name to use
  print *, "Please enter a valid KIM model name: "
  read(*,*) modelname

  ! Initialize the KIM object
  ier = kim_api_init_f(pkim, testname, modelname)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_init_f", ier)
     stop
  endif

  ! Find Model's cutoff  (This procedure needs to be improved in the future)
  !
  ! We need to get `cutoff' before we know how many atoms to use; so here we use 1 atom 
  ! Allocate memory via the KIM system
  N = 1
  call kim_api_allocate_f(pkim, N, ATypes, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_allocate_f", ier)
     stop
  endif
  ! call model's init routine
  ier = kim_api_model_init(pkim)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_model_init", ier)
     stop
  endif
  ! access the `cutoff' argument
  pcutoff = kim_api_get_data_f(pkim, "cutoff", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif
  ! determine number of atoms needed. (Need 2*cutoff, use 2.125*cutoff for saftey)
  CellsPerSide = ceiling((2.125d0*cutoff)/(MinSpacing))
  N = 4*(CellsPerSide**3);  N4=N
  ! tear it all down so we can put it back up
  call kim_api_model_destroy(pkim, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_model_destroy", ier)
     stop
  endif
  call kim_api_free(pkim, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_free", ier)
     stop
  endif

  ! Now setup for real.
  ier = kim_api_init_f(pkim, testname, modelname)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_init_f", ier)
     stop
  endif

  call kim_api_allocate_f(pkim, N, ATypes, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_allocate_f", ier)
     stop
  endif
  ! call model's init routine
  ier = kim_api_model_init(pkim)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_model_init", ier)
     stop
  endif


  ! determine which NBC scenerio to use
  pNBC_Method = kim_api_get_nbc_method(pkim, ier) ! don't forget to free
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_nbc_method", ier)
     stop
  endif
  if (index(NBC_Method,"MI-OPBC-H").eq.1) then
     nbc = 0
     halfflag = .true.
  elseif (index(NBC_Method,"MI-OPBC-F").eq.1) then
     nbc = 1
     halfflag = .false.
  else
     ier = 0
     call report_error(__LINE__, "Unknown NBC method", ier)
     return
  endif

  ! Unpack data from KIM object
  !
  pnAtoms = kim_api_get_data_f(pkim, "numberOfAtoms", ier);
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif

  pnAtomTypes = kim_api_get_data_f(pkim, "numberAtomTypes", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif

  patomTypesdum = kim_api_get_data_f(pkim, "atomTypes", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif
  call toIntegerArrayWithDescriptor1d(atomTypesdum, atomTypes, N4)

  pcoor = kim_api_get_data_f(pkim, "coordinates", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif
  call toRealArrayWithDescriptor2d(coordum, coords, DIM, N4)

  pcutoff = kim_api_get_data_f(pkim, "cutoff", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif

  if (nbc.le.1) then
     pboxlength = kim_api_get_data_f(pkim, "boxlength", ier)
     if (ier.le.0) then
        call report_error(__LINE__, "kim_api_get_data_f", ier)
        stop
     endif
  endif

  penergy = kim_api_get_data_f(pkim, "energy", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_data_f", ier)
     stop
  endif

  ! Set values
  numberOfAtoms   = N
  numberAtomTypes = ATypes
  atomTypes(:)    = kim_api_get_atypecode_f(pkim, "SPECIES_NAME_STR", ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_get_atypecode_f", ier)
     stop
  endif

  ! set up the periodic atom positions
  Spacings(1) = MinSpacing
  call create_FCC_configuration(Spacings(1), CellsPerSide, .true., coords, middleDum)
  boxlength(:)  = Spacings(1)*CellsPerSide

  ! compute neighbor lists
  allocate(neighborList(N+1, N))
  !
  call MI_OPBC_neighborlist(halfflag, N, coords, (cutoff+0.75), boxlength, neighborList)

  ! store pointers to neighbor list object and access function
  ier = kim_api_set_data_f(pkim, "neighObject", SizeOne, loc(neighborList))
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_set_data_f", ier)
     stop
  endif

  if (nbc.eq.0) then
     ier = kim_api_set_data_f(pkim, "get_half_neigh", SizeOne, loc(get_neigh_no_Rij))
     if (ier.le.0) then
        call report_error(__LINE__, "kim_api_set_data_f", ier)
        stop
     endif
  else
     ier = kim_api_set_data_f(pkim, "get_full_neigh", SizeOne, loc(get_neigh_no_Rij))
     if (ier.le.0) then
        call report_error(__LINE__, "kim_api_set_data_f", ier)
        stop
     endif
  endif

  ! print results to screen
  print *, "***********************************************************************************************"
  print *, "Results for KIM Model: ", modelname
  print *, "Using NBC: ", NBC_Method
  print *, ""

  ! Call model compute
  call kim_api_model_compute(pkim, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_model_compute", ier)
     stop
  endif
  Energies(1) = energy/N
  print *, "Energy/atom = ", Energies(1), "; Spacing = ", Spacings(1)

  ! setup and compute for max spacing
  Spacings(3) = MaxSpacing
  call create_FCC_configuration(Spacings(3), CellsPerSide, .true., coords, middleDum)
  boxlength(:) = Spacings(3)*CellsPerSide
  ! compute new neighbor lists (could be done more intelligently, I'm sure)
  call MI_OPBC_neighborlist(halfflag, N, coords, (cutoff+0.75), boxlength, neighborList)
  ! Call model compute
  call kim_api_model_compute(pkim, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_model_compute", ier)
     stop
  endif
  Energies(3) = energy/N
  print *, "Energy/atom = ", Energies(3), "; Spacing = ", Spacings(3)

  ! setup and compute for first intermediate spacing
  Spacings(2) = MinSpacing + (2.0 - Golden)*(MaxSpacing - MinSpacing)
  call create_FCC_configuration(Spacings(2), CellsPerSide, .true., coords, middleDum)
  boxlength(:) = Spacings(2)*CellsPerSide
  ! compute new neighbor lists (could be done more intelligently, I'm sure)
  call MI_OPBC_neighborlist(halfflag, N, coords, (cutoff+0.75), boxlength, neighborList)
  ! Call model compute
  call kim_api_model_compute(pkim, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_model_compute", ier)
     stop
  endif
  Energies(2) = energy/N
  print *, "Energy/atom = ", Energies(2), "; Spacing = ", Spacings(2)


  ! iterate until convergence.

  
  do while (abs(Spacings(3) - Spacings(1)) .gt. TOL)
     ! set new spacing
     Spacings(4) = (Spacings(1) + Spacings(3)) - Spacings(2)
     ! compute new atom coordinates based on new spacing
     call create_FCC_configuration(Spacings(4), CellsPerSide, .true., coords, middleDum)
     ! set new boxlength
     boxlength(:)  = Spacings(4)*CellsPerSide
     ! compute new neighbor lists (could be done more intelligently, I'm sure)
     call MI_OPBC_neighborlist(halfflag, N, coords, (cutoff+0.75), boxlength, neighborList)
     ! Call model compute
     call kim_api_model_compute(pkim, ier)
     if (ier.le.0) then
        call report_error(__LINE__, "kim_api_model_compute", ier)
        stop
     endif
     Energies(4) = energy/N
     print *, "Energy/atom = ", Energies(4), "; Spacing = ", Spacings(4)

     ! determine the new interval
     if (Energies(4) .lt. Energies(2)) then
        ! We want the right-hand interval
        Spacings(1) = Spacings(2); Energies(1) = Energies(2)
        Spacings(2) = Spacings(4); Energies(2) = Energies(4)
     else
        ! We want the left-hand interval
        Spacings(3) = Spacings(1); Energies(3) = Energies(1)
        Spacings(1) = Spacings(4); Energies(1) = Energies(4)
     endif
  enddo

  print *,
  print *,"Found minimum energy configuration to within", TOL
  print *,
  print *,"Energy/atom = ", Energies(2), "; Spacing = ", Spacings(2)

  ! Don't forget to free and/or deallocate
  call free(pNBC_Method) 
  deallocate(neighborList)

  call kim_api_model_destroy(pkim, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_model_destroy", ier)
     stop
  endif
  call kim_api_free(pkim, ier)
  if (ier.le.0) then
     call report_error(__LINE__, "kim_api_free", ier)
     stop
  endif

  stop
end program TEST_NAME_STR
