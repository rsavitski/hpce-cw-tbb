#include "fourier_transform.hpp"
#include "tbb/parallel_for.h"
#include "tbb/task_group.h"

#include <cmath>
#include <cassert>

namespace hpce
{

namespace rs5010
{
    
// parallelisation thresholds
const unsigned int M = 1<<12; // backwards fft bin scaling
const unsigned int K = 1<<12; // outer merge loop
const unsigned int L = 1<<10; // recursive splits

class fast_fourier_transform_opt
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

      // parallel or serial recursion
      if (m>L){
        tbb::task_group grp;

        grp.run( [=]{forwards_impl(m,wn*wn,pIn,2*sIn,pOut,sOut);} );
        grp.run( [=]{forwards_impl(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);} );
          
        grp.wait();
      }
      else {
        forwards_impl(m,wn*wn,pIn,2*sIn,pOut,sOut);
        forwards_impl(m,wn*wn,pIn+sIn,2*sIn,pOut+sOut*m,sOut);
      }


      auto outer_loop = [=](unsigned j0){
        std::complex<double> w=std::pow(wn,(j0*K));
        for (size_t j=j0*K; j<(j0+1)*K; ++j) {
          std::complex<double> t1 = w*pOut[m+j];
          std::complex<double> t2 = pOut[j]-t1;
          pOut[j] = pOut[j]+t1;                 
          pOut[j+m] = t2;                          
          w = w*wn;
        }
      };

      // parallel or serial merge loop
      if (m>K)
        tbb::parallel_for(size_t(0), m/K, outer_loop);
      else {
        std::complex<double> w=std::complex<double>(1.0, 0.0);
        for (size_t j=0; j<m; ++j) {
          std::complex<double> t1 = w*pOut[m+j];
          std::complex<double> t2 = pOut[j]-t1;
          pOut[j] = pOut[j]+t1;                 
          pOut[j+m] = t2;                          
          w = w*wn;
        }
      }
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
    
    // parallelise if sufficient problem size
    if (n>=M){
      tbb::parallel_for(size_t(0), n/M, [=](unsigned j){
        for(size_t i=0;i<M;++i)
          pOut[j*(M)+i]=pOut[j*(M)+i]*scale;
      });
    }
    else{
      for(size_t i=0;i<n;i++){
        pOut[i]=pOut[i]*scale;
      }
    }
	}
	
public:
	virtual std::string name() const
	{ return "hpce.rs5010.fast_fourier_transform_opt"; }
	
	virtual bool is_quadratic() const
	{ return false; }
};

std::shared_ptr<fourier_transform> Create_fast_fourier_transform_opt()
{
	return std::make_shared<fast_fourier_transform_opt>();
}

}; // namespace rs5010

}; // namespace hpce
