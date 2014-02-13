#!/bin/bash

PROCNUM=4
MAXTIME=5

#for x in hpce.direct_fourier_transform hpce.fast_fourier_transform hpce.rs5010.direct_fourier_transform_parfor hpce.rs5010.fast_fourier_transform_combined hpce.rs5010.fast_fourier_transform_opt hpce.rs5010.fast_fourier_transform_parfor hpce.rs5010.fast_fourier_transform_taskgroup

#for x in hpce.fast_fourier_transform hpce.rs5010.fast_fourier_transform_parfor hpce.rs5010.fast_fourier_transform_taskgroup hpce.rs5010.fast_fourier_transform_combined hpce.rs5010.fast_fourier_transform_opt
#for x in hpce.rs5010.fast_fourier_transform_combined hpce.rs5010.fast_fourier_transform_opt hpce.fast_fourier_transform
for x in hpce.rs5010.fast_fourier_transform_opt
do
  echo $x
  (bin/time_fourier_transform $x $PROCNUM $MAXTIME) > ../tables/${x}_${PROCNUM}

#  sed -i '$ d' ../tables/${x}_${PROCNUM}
done
