#include "sfe_ism_shim.h"
#include "sfe_lsm6dsv16x.h"


// Initializes the interfacing struct for STMicroelectronic's library. 
void initCtx(void* handle, stmdev_ctx_t* dev){

	dev->handle = handle; 	
	dev->write_reg = sfeISMWrite;
	dev->read_reg = sfeISMRead;
}

// Points to the write method in the LSM6DSV16X Arduino Library
int32_t sfeISMWrite(void* fTarget, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
	return (((QwDevLSM6DSV16X*)fTarget)->writeRegisterRegion(reg, (uint8_t*)bufp, len));
}

// Points to the read method in the LSM6DSV16X Arduino Library
int32_t sfeISMRead(void* fTarget, uint8_t reg, uint8_t *bufp, uint16_t len)
{
    return (((QwDevLSM6DSV16X*)fTarget)->readRegisterRegion(reg, bufp, len));
}



