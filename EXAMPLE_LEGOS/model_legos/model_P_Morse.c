/*******************************************************************************
*
*  MODEL_NAME_STR
*
*  Morse pair potential model for SPECIES_NAME_STR
*  REFERENCE_STR
*  modified to have smooth cutoff
*
*  Author: Valeriu Smirichinski, Ryan S. Elliott, Ellad B. Tadmor
*
*  Copyright 2011 Ellad B. Tadmor, Ryan S. Elliott, and James P. Sethna
*  All rights reserved.
*
*******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "KIMserviceC.h"

#define DIM 3

/* Define prototypes for model init */
void MODEL_NAME_LC_STR_init_(void* km);

/* Define prototypes for model reinit, compute, and destroy */
static void reinit(void* km);
static void destroy(void* km);
static void compute(void* km, int* ier);
static void report_error(int line, char* str, int status);


static void pair(double* epsilon, double* C, double* Rzero,
                 double* A1, double* A2, double* A3,
                 double R, double* phi, double* dphi, double* d2phi)
{
   /* local variables */
   double rsq;
   double ep;
   double ep2;

   ep  = exp(-(*C)*(R-*Rzero));
   ep2 = ep*ep;

   *phi   = (*epsilon)*( -ep2 + 2.0*ep ) + (*A1)*(R*R) + (*A2)*R + (*A3);
   *dphi  = 2.0*(*epsilon)*(*C)*( -ep + ep2 ) + 2.0*(*A1)*R + (*A2);
   *d2phi = 2.0*(*epsilon)*(*C)*(*C)*( -2.0*ep2 + ep ) + 2.0*(*A1);

   return;
}

/* compute function */
static void compute(void* km, int* ier)
{
   /* local variables */
   intptr_t* pkim = *((intptr_t**) km);
   double R;
   double Rsqij;
   double phi;
   double dphi;
   double d2phi;
   double dx[DIM];
   int i;
   int j;
   int jj;
   int k;
   int numOfAtomNeigh;
   int requestedAtom;
   int currentAtom;
   int comp_force;
   int comp_energyPerAtom;
   int comp_virial;
   int IterOrLoca;
   int NBC;
   char* NBCstr;

   intptr_t* nAtoms;
   int* nAtomTypes;
   int* atomTypes;
   double* cutoff;
   double* epsilon;
   double* C;
   double* Rzero;
   double* A1;
   double* A2;
   double* A3;
   double* cutsq;
   double* Rij;
   double* coords;
   double* energy;
   double* force;
   double* energyPerAtom;
   double* virial;
   int* neighListOfCurrentAtom;
   double* boxlength;
   
   
   /* determine NBC */
   /* NBC = even -> Full, NBC = odd -> Half */
   NBCstr = KIM_API_get_NBC_method(pkim, ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_NBC_method", *ier);
      return;
   }
   if (!strcmp("CLUSTER",NBCstr))
      NBC = 0;
   else if (!strcmp("MI-OPBC-H",NBCstr))
      NBC = 1;
   else if (!strcmp("MI-OPBC-F",NBCstr))
      NBC = 2;
   else if (!strcmp("NEIGH-PURE-H",NBCstr))
      NBC = 3;
   else if (!strcmp("NEIGH-PURE-F",NBCstr))
      NBC = 4;
   else if (!strcmp("NEIGH-RVEC-F",NBCstr))
      NBC = 6;
   else
   {
      *ier = 0;
      report_error(__LINE__, "Unknown NBC type", *ier);
      return;
   }
   free(NBCstr);

   /* determine mode */
   if (NBC > 0)
   {
      IterOrLoca = KIM_API_get_neigh_mode(pkim, ier);
      if (1 > *ier)
      {
         report_error(__LINE__, "KIM_API_get_neigh_mode", *ier);
         return;
      }
   }


   /* unpack data from KIM object */
   nAtoms = (intptr_t*) KIM_API_get_data(pkim, "numberOfAtoms", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   nAtomTypes = (int*) KIM_API_get_data(pkim, "numberAtomTypes", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   atomTypes= (int*) KIM_API_get_data(pkim, "atomTypes", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   cutoff = (double*) KIM_API_get_data(pkim, "cutoff", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   epsilon = (double*) KIM_API_get_data(pkim, "PARAM_FREE_epsilon", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   C = (double*) KIM_API_get_data(pkim, "PARAM_FREE_C", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   Rzero = (double*) KIM_API_get_data(pkim, "PARAM_FREE_Rzero", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   A1 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A1", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   A2 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A2", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   A3 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A3", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   cutsq = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_cutsq", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   energy = (double*) KIM_API_get_data(pkim, "energy", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   coords = (double*) KIM_API_get_data(pkim, "coordinates", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_get_data", *ier);
      return;
   }
   if ((NBC == 1) || (NBC == 2))
   {
      boxlength = (double*) KIM_API_get_data(pkim, "boxlength", ier);
      if (1 > *ier)
      {
         report_error(__LINE__, "KIM_API_get_data", *ier);
         return;
      }
   }

   /* check to see if we have been asked to compute the forces, energyPerAtom, and virial */
   comp_force = KIM_API_isit_compute(pkim, "forces", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_isit_compute", *ier);
      return;
   }
   comp_energyPerAtom = KIM_API_isit_compute(pkim, "energyPerAtom", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_isit_compute", *ier);
      return;
   }
   comp_virial = KIM_API_isit_compute(pkim, "virial", ier);
   if (1 > *ier)
   {
      report_error(__LINE__, "KIM_API_isit_compute", *ier);
      return;
   }

   /* check to be sure the atom types are correct by comparing the provided species */
   /* codes to the value given here (which should be the same as that given in the  */
   /* .kim file).                                                                   */
   *ier = 0; /* assume an error */
   for (i = 0; i < *nAtoms; ++i)
   {
      if ( SPECIES_CODE_STR != atomTypes[i])
      {
         report_error(__LINE__, "Wrong atomType", i);
         return;
      }
   }
   *ier = 1; /* everything is ok */

   /* initialize potential energies, forces, and virial term */
   if (comp_energyPerAtom)
   {
      energyPerAtom = (double*) KIM_API_get_data(pkim, "energyPerAtom", ier);
      if (1 > *ier)
      {
         report_error(__LINE__, "KIM_API_get_data", *ier);
         return;
      }
      for (i = 0; i < *nAtoms; ++i)
      {
         energyPerAtom[i] = 0.0;
    
      }
   }
   else
   {
      *energy = 0.0;
   }

   if (comp_force)
   {
      force = (double*) KIM_API_get_data(pkim, "forces", ier);
      if (1 > *ier)
      {
         report_error(__LINE__, "KIM_API_get_data", *ier);
         return;
      }
      for (i = 0; i < *nAtoms; ++i)
      {
         for (k = 0; k < DIM; ++k)
         {
            force[i*DIM + k] = 0.0;
         }
      }
   }

   if (comp_virial)
   {
      virial = (double*) KIM_API_get_data(pkim, "virial", ier);
      if (1 > *ier)
      {
         report_error(__LINE__, "KIM_API_get_data", *ier);
         return;
      }
      *virial = 0.0;
   }

   /* Ready to setup for and perform the computation */

   /* branch based on NBC */
   if (0 == NBC) /* CLUSTER */
   {
      /* Loop over all pairs of atoms */
      for (i = 0; i < *nAtoms; ++i)
      {
         for (j = i+1; j < *nAtoms; ++j) /* use a half-neighbor approach */
         {
            Rsqij = 0.0;
            for (k = 0; k < DIM; ++k)
            {
               Rsqij += (coords[j*DIM + k] - coords[i*DIM + k])*(coords[j*DIM + k] - coords[i*DIM + k]);
            }

            if (Rsqij < *cutsq)
            {
               R = sqrt(Rsqij);
               /* compute pair potential */
               pair(epsilon, C, Rzero, A1, A2, A3, R, &phi, &dphi, &d2phi);

               /* accumlate energy */
               if (comp_energyPerAtom)
               {
                  energyPerAtom[i] += 0.5*phi;
                  energyPerAtom[j] += 0.5*phi;
               }
               else
               {
                  *energy += phi;
               }

               /* accumulate virial */
               if (comp_virial)
               {
                  *virial += R*dphi;
               }

               /* accumulate force */
               if (comp_force)
               {
                  for (k = 0; k < DIM; ++k)
                  {
                     force[i*DIM + k] += dphi*(coords[j*DIM + k] - coords[i*DIM + k])/R;
                     force[j*DIM + k] -= dphi*(coords[j*DIM + k] - coords[i*DIM + k])/R;
                  }
               }
            }
         }
      }
   }
   else /* everything else */
   {
      if (1 == IterOrLoca) /* Iterator mode */
      {
         /* reset neighbor iterator */
         if (0 == (NBC%2)) /* full list */
         {
            *ier = KIM_API_get_full_neigh(pkim, 0, 0, &currentAtom,
                                          &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
         }
         else /* half list */
         {
            *ier = KIM_API_get_half_neigh(pkim, 0, 0, &currentAtom,
                                          &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
         }
         if (2 != *ier)
         {
            *ier = 0;
            return;
         }

         if (0 == (NBC%2)) /* full list */
         {
            *ier = KIM_API_get_full_neigh(pkim, 0, 1, &currentAtom,
                                          &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
         }
         else /* half list */
         {
            *ier = KIM_API_get_half_neigh(pkim, 0, 1, &currentAtom,
                                          &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
         }
         while (1 == *ier)
         {
            /* loop over the neighbors of currentAtom */
            for (jj = 0; jj < numOfAtomNeigh; ++ jj)
            {
               /* compute square distance */
               Rsqij = 0.0;
               for (k = 0; k < DIM; ++k)
               {
                  if (NBC < 5) /* MI-OPBC-H/F & NEIGH-PURE-H/F */
                  {
                     dx[k] = coords[neighListOfCurrentAtom[jj]*DIM + k] - coords[currentAtom*DIM + k];
                     
                     if ((NBC < 3) && (fabs(dx[k]) > 0.5*boxlength[k])) /* MI-OPBC-H/F */
                     {
                        dx[k] -= (dx[k]/fabs(dx[k]))*boxlength[k];
                     }
                  }
                  else /* NEIGH-RVEC-F */
                  {
                     dx[k] = Rij[jj*DIM + k];
                  }
                  
                  Rsqij += dx[k]*dx[k];
               }
               
               /* particles are interacting ? */
               if (Rsqij < *cutsq)
               {
                  R = sqrt(Rsqij);
                  /* compute pair potential */
                  pair(epsilon, C, Rzero, A1, A2, A3, R, &phi, &dphi, &d2phi);
                  
                  /* accumulate energy */
                  if (comp_energyPerAtom)
                  {
                     energyPerAtom[currentAtom] += 0.5*phi;
                     if (1 == (NBC%2)) energyPerAtom[neighListOfCurrentAtom[jj]] += 0.5*phi;
                  }
                  else
                  {
                     *energy += ( (0 == (NBC%2)) ? (0.5*phi) : (phi) );
                  }
                  
                  /* accumulate virial */
                  if (comp_virial)
                  {
                     *virial += ( (0 == (NBC%2)) ? 0.5 : 1.0 )*R*dphi;
                  }
                  
                  /* accumulate force */
                  if (comp_force)
                  {
                     for (k = 0; k < DIM; ++k)
                     {
                        force[currentAtom*DIM + k] += dphi*dx[k]/R;
                        if (1 == (NBC%2)) force[neighListOfCurrentAtom[jj]*DIM + k] -= dphi*dx[k]/R;
                     }
                  }
               }
            }
            
            /* increment iterator */
            if (0 == (NBC%2)) /* full list */
            {
               *ier = KIM_API_get_full_neigh(pkim, 0, 1, &currentAtom,
                                             &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
            }
            else /* half list */
            {
               *ier = KIM_API_get_half_neigh(pkim, 0, 1, &currentAtom,
                                             &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
            }
         }
      }
      else if (2 == IterOrLoca) /* Locator mode */
      {
         /* loop over atoms */
         for (i = 0; i < *nAtoms; ++i)
         {
            /* get neighbor list for atom i */
            if (0 == (NBC%2)) /* full list */
            {
               *ier = KIM_API_get_full_neigh(pkim, 1, i, &currentAtom,
                                             &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
            }
            else /* half list */
            {
               *ier = KIM_API_get_half_neigh(pkim, 1, i, &currentAtom,
                                             &numOfAtomNeigh, &neighListOfCurrentAtom, &Rij);
            }
            
            /* loop over the neighbors of currentAtom */
            for (jj = 0; jj < numOfAtomNeigh; ++ jj)
            {
               /* compute square distance */
               Rsqij = 0.0;
               for (k = 0; k < DIM; ++k)
               {
                  if (NBC < 5) /* MI-OPBC-H/F & NEIGH-PURE-H/F */
                  {
                     dx[k] = coords[neighListOfCurrentAtom[jj]*DIM + k] - coords[currentAtom*DIM + k];
                     
                     if ((NBC < 3) && (fabs(dx[k]) > 0.5*boxlength[k])) /* MI-OPBC-H/F */
                     {
                        dx[k] -= (dx[k]/fabs(dx[k]))*boxlength[k];
                     }
                  }
                  else /* NEIGH-RVEC-F */
                  {
                     dx[k] = Rij[jj*DIM + k];
                  }
                  
                  Rsqij += dx[k]*dx[k];
               }
               
               /* particles are interacting ? */
               if (Rsqij < *cutsq)
               {
                  R = sqrt(Rsqij);
                  /* compute pair potential */
                  pair(epsilon, C, Rzero, A1, A2, A3, R, &phi, &dphi, &d2phi);
                  
                  /* accumulate energy */
                  if (comp_energyPerAtom)
                  {
                     energyPerAtom[currentAtom] += 0.5*phi;
                     if (1 == (NBC%2)) energyPerAtom[neighListOfCurrentAtom[jj]] += 0.5*phi;
                  }
                  else
                  {
                     *energy += ( (0 == (NBC%2)) ? (0.5*phi) : (phi) );
                  }
                  
                  /* accumulate virial */
                  if (comp_virial)
                  {
                     *virial += ( (0 == (NBC%2)) ? 0.5 : 1.0 )*R*dphi;
                  }
                  
                  /* accumulate force */
                  if (comp_force)
                  {
                     for (k = 0; k < DIM; ++k)
                     {
                        force[currentAtom*DIM + k] += dphi*dx[k]/R;
                        if (1 == (NBC%2)) force[neighListOfCurrentAtom[jj]*DIM + k] -= dphi*dx[k]/R;
                     }
                  }
               }
            }

         }
      }
      else /* unsupported IterOrLoca mode returned from KIM_API_get_neigh_mode() */
      {
         report_error(__LINE__, "KIM_API_get_neigh_mode", IterOrLoca);
         exit(-1);
      }
   }


   /* perform final tasks */
   
   if (comp_virial)
   {
      *virial = -*virial/( (double) DIM); /* definition of virial term */
   }

   if (comp_energyPerAtom)
   {
      *energy = 0.0;
      for (i = 0; i < *nAtoms; ++i)
      {
         *energy += energyPerAtom[i];
      }
   }

   /* everything is great */
   *ier = 1;
   return;
}


/* Reinitialization function */
static void reinit(void *km)
{
   /* Local variables */
   intptr_t* pkim = *((intptr_t**) km);
   double* model_cutoff;
   double* model_epsilon;
   double* model_C;
   double* model_Rzero;
   double* model_Pcutoff;
   double* model_A1;
   double* model_A2;
   double* model_A3;
   double* model_cutsq;
   int ier;
   double ep;
   double ep2;

   /* get (changed) parameters from KIM object */

   /* get parameter cutoff from KIM object */
   model_Pcutoff = (double*) KIM_API_get_data(pkim, "PARAM_FREE_cutoff", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }

   /* get epsilon from KIM object */
   model_epsilon = (double*) KIM_API_get_data(pkim, "PARAM_FREE_epsilon", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }

   /* get C from KIM object */
   model_C = (double*) KIM_API_get_data(pkim, "PARAM_FREE_C", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }

   /* get Rzero from KIM object */
   model_Rzero = (double*) KIM_API_get_data(pkim, "PARAM_FREE_Rzero", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }

   /* set new values in KIM object */
   

   /* store model cutoff in KIM object */
   model_cutoff = (double*) KIM_API_get_data(pkim, "cutoff", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   *model_cutoff = *model_Pcutoff;

   ep  = exp(-(*model_C)*((*model_cutoff) - (*model_Rzero)));
   ep2 = ep*ep;
   
   /* store model_A1 in KIM object */
   model_A1 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A1", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   /* set value of parameter A1 */
   *model_A1 = -((*model_epsilon)*(*model_C)*(*model_C)*( -2.0*ep2 + ep ));

   /* store model_A2 in KIM object */
   model_A2 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A2", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   /* set value of parameter A2 */
   *model_A2 = -( 2.0*(*model_epsilon)*(*model_C)*( -ep + ep2 )
                 +2.0*(*model_A1)*(*model_cutoff) );

   /* store model_A3 in KIM object */
   model_A3 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A3", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   /* set value of parameter A3 */
   *model_A3 = -( (*model_epsilon)*( -ep2 + 2.0*ep )
                 +(*model_A1)*(*model_cutoff)*(*model_cutoff)
                 +(*model_A2)*(*model_cutoff) );

   /* store model_cutsq in KIM object */
   model_cutsq = KIM_API_get_data(pkim, "PARAM_FIXED_cutsq", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   /* set value of parameter cutsq */
   *model_cutsq = (*model_cutoff)*(*model_cutoff);

   return;
}

/* destroy function */
static void destroy(void *km)
{
   /* Local variables */
   intptr_t* pkim = *((intptr_t**) km);
   double* model_cutoff;
   double* model_epsilon;
   double* model_C;
   double* model_Rzero;
   double* model_Pcutoff;
   double* model_A1;
   double* model_A2;
   double* model_A3;
   double* model_cutsq;
   int ier;

   /* get and free parameter cutoff from KIM object */
   model_Pcutoff = (double*) KIM_API_get_data(pkim, "PARAM_FREE_cutoff", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   free(model_Pcutoff);

   /* get and free epsilon from KIM object */
   model_epsilon = (double*) KIM_API_get_data(pkim, "PARAM_FREE_epsilon", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   free(model_epsilon);

   /* get and free C from KIM object */
   model_C = (double*) KIM_API_get_data(pkim, "PARAM_FREE_C", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   free(model_C);

   /* get and free model_A1 in KIM object */
   model_A1 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A1", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   free(model_A1);

   /* get and free model_A2 in KIM object */
   model_A2 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A2", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   free(model_A2);

   /* get and free model_A3 in KIM object */
   model_A3 = (double*) KIM_API_get_data(pkim, "PARAM_FIXED_A3", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   free(model_A3);
   
   /* get and free model_cutsq in KIM object */
   model_cutsq = KIM_API_get_data(pkim, "PARAM_FIXED_cutsq", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   free(model_cutsq);

   return;
}


/* Initialization function */
void MODEL_NAME_LC_STR_init_(void *km)
{
   /* Local variables */
   intptr_t* pkim = *((intptr_t**) km);
   double* model_cutoff;
   double* model_epsilon;
   double* model_C;
   double* model_Rzero;
   double* model_Rsq;
   double* model_Pcutoff;
   double* model_A1;
   double* model_A2;
   double* model_A3;
   double* model_cutsq;
   int ier;
   double ep;
   double ep2;

   /* store pointer to compute function in KIM object */
   if (! KIM_API_set_data(pkim, "compute", 1, (void*) &compute))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }

   /* store pointer to reinit function in KIM object */
   if (! KIM_API_set_data(pkim, "reinit", 1, (void*) &reinit))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }

   /* store pointer to destroy function in KIM object */
   if (! KIM_API_set_data(pkim, "destroy", 1, (void*) &destroy))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* store model cutoff in KIM object */
   model_cutoff = (double*) KIM_API_get_data(pkim, "cutoff", &ier);
   if (1 > ier)
   {
      report_error(__LINE__, "KIM_API_get_data", ier);
      exit(1);
   }
   CUTOFF_VALUE_STR

   /* allocate memory for parameter cutoff and store value */
   model_Pcutoff = (double*) malloc(1*sizeof(double));
   if (NULL == model_Pcutoff)
   {
      report_error(__LINE__, "malloc", ier);
      exit(1);
   }
   /* store model_Pcutoff in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FREE_cutoff", 1, (void*) model_Pcutoff))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* set value of parameter cutoff */
   *model_Pcutoff = *model_cutoff;

   /* allocate memory for epsilon and store value */
   model_epsilon = (double*) malloc(1*sizeof(double));
   if (NULL == model_epsilon)
   {
      report_error(__LINE__, "malloc", ier);
      exit(1);
   }
   /* store model_epsilon in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FREE_epsilon", 1, (void*) model_epsilon))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* set value of epsilon */
   *model_epsilon = EPSILON_VALUE_STR

   /* allocate memory for C and store value */
   model_C = (double*) malloc(1*sizeof(double));
   if (NULL == model_C)
   {
      report_error(__LINE__, "malloc", ier);
      exit(1);
   }
   /* store model_C in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FREE_C", 1, (void*) model_C))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* set value of C */
   C_VALUE_STR

   /* allocate memory for Rzero and store value */
   model_Rzero = (double*) malloc(1*sizeof(double));
   if (NULL == model_Rzero)
   {
      report_error(__LINE__, "malloc", ier);
      exit(1);
   }
   /* store model_Rzero in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FREE_Rzero", 1, (void*) model_Rzero))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* set value of Rzero */
   RZERO_VALUE_STR

   ep  = exp(-(*model_C)*((*model_cutoff) - (*model_Rzero)));
   ep2 = ep*ep;

   /* allocate memory for parameter A1 and store value */
   model_A1 = (double*) malloc(1*sizeof(double));
   if (NULL == model_A1)
   {
      report_error(__LINE__, "malloc", ier);
      exit(1);
   }
   /* store model_A1 in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FIXED_A1", 1, (void*) model_A1))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* set value of parameter A1 */
   *model_A1 = -((*model_epsilon)*(*model_C)*(*model_C)*( -2.0*ep2 + ep ));

   /* allocate memory for parameter A2 and store value */
   model_A2 = (double*) malloc(1*sizeof(double));
   if (NULL == model_A2)
   {
      report_error(__LINE__, "malloc", ier);
      exit(1);
   }
   /* store model_A2 in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FIXED_A2", 1, (void*) model_A2))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* set value of parameter A2 */
   *model_A2 = -( 2.0*(*model_epsilon)*(*model_C)*( -ep + ep2 )
                 +2.0*(*model_A1)*(*model_cutoff) );

   /* allocate memory for parameter A3 and store value */
   model_A3 = (double*) malloc(1*sizeof(double));
   if (NULL == model_A3)
   {
      report_error(__LINE__, "malloc", ier);
      exit(1);
   }
   /* store model_A3 in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FIXED_A3", 1, (void*) model_A3))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* set value of parameter A3 */
   *model_A3 = -( (*model_epsilon)*( -ep2 + 2.0*ep )
                 +(*model_A1)*(*model_cutoff)*(*model_cutoff)
                 +(*model_A2)*(*model_cutoff) );

   /* allocate memory for parameter cutsq and store value */
   model_cutsq = (double*) malloc(1*sizeof(double));
   if (NULL == model_cutsq)
   {
      report_error(__LINE__, "malloc", ier);
      exit(1);
   }
   /* store model_cutsq in KIM object */
   if (! KIM_API_set_data(pkim, "PARAM_FIXED_cutsq", 1, (void*) model_cutsq))
   {
      report_error(__LINE__, "KIM_API_set_data", ier);
      exit(1);
   }
   /* set value of parameter cutsq */
   *model_cutsq = (*model_cutoff)*(*model_cutoff);

   return;
}

static void report_error(int line, char* str, int status)
{
   printf("* ERROR at line %i in %s: %s. kimerror = %i\n", line, __FILE__, str, status);
}
