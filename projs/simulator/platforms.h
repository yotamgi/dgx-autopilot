#ifndef PLATFORMS_H_
#define PLATFORMS_H_

#include "plain.h"

namespace simulator {
namespace platforms {

const PlainParams dgx_platform(
		 "media/pf-cessna-182.x",
		 "media/pf-cessna-182.bmp",
		 irr::core::vector3df(1., 1., 1.),
		 irr::core::vector3df(0., 180., 0.),
		 500.0f,
		 500.0f,
		 500.0f,
		 6.5f,  	// mass
		 40.0f,  	// engine power
		 0.15f, 	// drag
		 1.4f, 		// wing area
		 -3.,  		// lift
		 30.,		// sideslide to yaw effect strenth
		 20.); 		// dihedral effect strenth


const PlainParams cecsna(
    	 "media/pf-cessna-182.x",
    	 "media/pf-cessna-182.bmp",
    	 irr::core::vector3df(4.0f, 4.0f, 4.0f),
    	 irr::core::vector3df(0., 180., 0.),
    	 200.0f,
    	 60.0f,
    	 100.0f,
    	 1000.0f,  	// mass
    	 3000.0f,  	// engine power
    	 0.8f, 		// drag
    	 13.0f, 	// wing area
    	 -5., 		// lift
		 30.,		// sideslide to yaw effect strenth
		 20.); 		// dihedral effect strenth

}  // namespace platforms
}  // namespace simulator

#endif /* PLATFORMS_H_ */
