#include "fourier_transform.hpp"

namespace hpce{

// Declare factory functions which are implemented elsewhere.
std::shared_ptr<fourier_transform> Create_fast_fourier_transform();
std::shared_ptr<fourier_transform> Create_fast_fourier_transform_taskgroup();
std::shared_ptr<fourier_transform> Create_direct_fourier_transform();
std::shared_ptr<fourier_transform> Create_direct_fourier_transform_parfor();
	
namespace rs5010{
	std::shared_ptr<fourier_transform> Create_direct_fourier_transform_parfor();
	std::shared_ptr<fourier_transform> Create_fast_fourier_transform_taskgroup();
	std::shared_ptr<fourier_transform> Create_fast_fourier_transform_parfor();
}



void fourier_transform::RegisterDefaultFactories()
{
	static const unsigned MYSTERIOUS_LINE=0; // Don't remove me!
	
	RegisterTransformFactory("hpce.fast_fourier_transform", Create_fast_fourier_transform);
	RegisterTransformFactory("hpce.direct_fourier_transform", Create_direct_fourier_transform);
  RegisterTransformFactory("hpce.rs5010.direct_fourier_transform_parfor", hpce::rs5010::Create_direct_fourier_transform_parfor);
	RegisterTransformFactory("hpce.rs5010.fast_fourier_transform_taskgroup", hpce::rs5010::Create_fast_fourier_transform_taskgroup);
	RegisterTransformFactory("hpce.rs5010.fast_fourier_transform_parfor", hpce::rs5010::Create_fast_fourier_transform_parfor);
}
	
}; // namespace hpce
