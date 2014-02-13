#include "fourier_transform.hpp"
#include "tbb/parallel_for.h"

#include <cmath>
#include <cassert>

namespace hpce
{

namespace rs5010
{

class fast_fourier_transform_parfor
	: public fourier_transform
{
protected:
	/* Standard radix-2 FFT only supports binary power lengths */
	virtual size_t calc_padded_size(size_t n) const
	{
		assert(n!=0);
		
		size_t ret=1;
		while(ret<n){
			ret<<=1;
		}
		
		return ret;
	}

	virtual void forwards_impl(
		size_t n,	const std::complex<double> &wn,
		const std::complex<double> *pIn, size_t sIn,
		std::complex<double> *pOut, size_t sOut
	) const 
	{
		assert(n>0);
		
		if (n == 1){
			pOut[0] = pIn[0];
		}else if (n == 2){
			pOut[0] = pIn[0]+pIn[sIn];
			pOut[sOut] = pIn[0]-pIn[sIn];
		}else{
			size_t m = n/2;

			forwards_impl(m,wn*wn,pIn,2*sIn,pOut,sOut);
			forwards_impl(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
			 
      size_t K = (m<8)?m:8;

      auto outer_loop = [=](unsigned j0){
        std::complex<double> w=std::pow(wn,(j0*K));
        for (size_t j1=0; j1<K; j1++){
          size_t j=j0*K+j1;
          std::complex<double> t1 = w*pOut[m+j];
          std::complex<double> t2 = pOut[j]-t1;
          pOut[j] = pOut[j]+t1;                 
          pOut[j+m] = t2;                          
          w = w*wn;
        }
      };
      tbb::parallel_for(size_t(0), m/K, outer_loop);
    }
  }
	
	virtual void backwards_impl(
		size_t n,	const std::complex<double> &wn,
		const std::complex<double> *pIn, size_t sIn,
		std::complex<double> *pOut, size_t sOut
	) const 
	{
		complex_t reverse_wn=1.0/wn;
		forwards_impl(n, reverse_wn, pIn, sIn, pOut, sOut);
		
		double scale=1.0/n;
		for(size_t i=0;i<n;i++){
			pOut[i]=pOut[i]*scale;
		}
	}
	
public:
	virtual std::string name() const
	{ return "hpce.rs5010.fast_fourier_transform_parfor"; }
	
	virtual bool is_quadratic() const
	{ return false; }
};

std::shared_ptr<fourier_transform> Create_fast_fourier_transform_parfor()
{
	return std::make_shared<fast_fourier_transform_parfor>();
}

}; // namespace rs5010

}; // namespace hpce
