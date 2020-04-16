//
// win_aligned_alloc.h
//

#ifndef __WIN_ALIGNED_ALLOC_H_
#define __WIN_ALIGNED_ALLOC_H_

    // Windows build uses a different aligned_alloc
    #ifdef _WIN32
	#define aligned_alloc(alignment,size) _aligned_malloc (size, alignment)
    #endif

#endif
