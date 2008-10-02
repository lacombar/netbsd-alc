#include "drmP.h"
#include "drm.h"
#include "nouveau_drv.h"
#include "nouveau_drm.h"

/*
 * NV20
 * -----
 * There are 3 families :
 * NV20 is 0x10de:0x020*
 * NV25/28 is 0x10de:0x025* / 0x10de:0x028*
 * NV2A is 0x10de:0x02A0
 *
 * NV30
 * -----
 * There are 3 families :
 * NV30/31 is 0x10de:0x030* / 0x10de:0x031*
 * NV34 is 0x10de:0x032*
 * NV35/36 is 0x10de:0x033* / 0x10de:0x034*
 *
 * Not seen in the wild, no dumps (probably NV35) :
 * NV37 is 0x10de:0x00fc, 0x10de:0x00fd
 * NV38 is 0x10de:0x0333, 0x10de:0x00fe
 *
 */

#define NV20_GRCTX_SIZE (3580*4)
#define NV25_GRCTX_SIZE (3529*4)
#define NV2A_GRCTX_SIZE (3500*4)

#define NV30_31_GRCTX_SIZE (24392)
#define NV34_GRCTX_SIZE    (18140)
#define NV35_36_GRCTX_SIZE (22396)

static void nv20_graph_context_init(struct drm_device *dev,
                                    struct nouveau_gpuobj *ctx)
{
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	int i;
/*
write32 #1 block at +0x00740adc NV_PRAMIN+0x40adc of 3369 (0xd29) elements:
+0x00740adc: ffff0000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740afc: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740b1c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740b3c: 00000000 0fff0000 0fff0000 00000000 00000000 00000000 00000000 00000000
+0x00740b5c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740b7c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740b9c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740bbc: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740bdc: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740bfc: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

+0x00740c1c: 00000101 00000000 00000000 00000000 00000000 00000111 00000000 00000000
+0x00740c3c: 00000000 00000000 00000000 44400000 00000000 00000000 00000000 00000000
+0x00740c5c: 00000000 00000000 00000000 00000000 00000000 00000000 00030303 00030303
+0x00740c7c: 00030303 00030303 00000000 00000000 00000000 00000000 00080000 00080000
+0x00740c9c: 00080000 00080000 00000000 00000000 01012000 01012000 01012000 01012000
+0x00740cbc: 000105b8 000105b8 000105b8 000105b8 00080008 00080008 00080008 00080008
+0x00740cdc: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740cfc: 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000
+0x00740d1c: 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000
+0x00740d3c: 00000000 00000000 4b7fffff 00000000 00000000 00000000 00000000 00000000

+0x00740d5c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740d7c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740d9c: 00000001 00000000 00004000 00000000 00000000 00000001 00000000 00040000
+0x00740dbc: 00010000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740ddc: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
...
*/
	INSTANCE_WR(ctx, (0x33c/4)+0, 0xffff0000);
	INSTANCE_WR(ctx, (0x33c/4)+25, 0x0fff0000);
	INSTANCE_WR(ctx, (0x33c/4)+26, 0x0fff0000);
	INSTANCE_WR(ctx, (0x33c/4)+80, 0x00000101);
	INSTANCE_WR(ctx, (0x33c/4)+85, 0x00000111);
	INSTANCE_WR(ctx, (0x33c/4)+91, 0x44400000);
	for (i = 0; i < 4; ++i)
		INSTANCE_WR(ctx, (0x33c/4)+102+i, 0x00030303);
	for (i = 0; i < 4; ++i)
		INSTANCE_WR(ctx, (0x33c/4)+110+i, 0x00080000);
	for (i = 0; i < 4; ++i)
		INSTANCE_WR(ctx, (0x33c/4)+116+i, 0x01012000);
	for (i = 0; i < 4; ++i)
		INSTANCE_WR(ctx, (0x33c/4)+120+i, 0x000105b8);
	for (i = 0; i < 4; ++i)
		INSTANCE_WR(ctx, (0x33c/4)+124+i, 0x00080008);
	for (i = 0; i < 16; ++i)
		INSTANCE_WR(ctx, (0x33c/4)+136+i, 0x07ff0000);
	INSTANCE_WR(ctx, (0x33c/4)+154, 0x4b7fffff);
	INSTANCE_WR(ctx, (0x33c/4)+176, 0x00000001);
	INSTANCE_WR(ctx, (0x33c/4)+178, 0x00004000);
	INSTANCE_WR(ctx, (0x33c/4)+181, 0x00000001);
	INSTANCE_WR(ctx, (0x33c/4)+183, 0x00040000);
	INSTANCE_WR(ctx, (0x33c/4)+184, 0x00010000);

/*
...
+0x0074239c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x007423bc: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x007423dc: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x007423fc: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
...
+0x00742bdc: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742bfc: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742c1c: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742c3c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
...
*/
	for (i = 0; i < 0x880; i += 0x10) {
		INSTANCE_WR(ctx, ((0x1c1c + i)/4)+0, 0x10700ff9);
		INSTANCE_WR(ctx, ((0x1c1c + i)/4)+1, 0x0436086c);
		INSTANCE_WR(ctx, ((0x1c1c + i)/4)+2, 0x000c001b);
	}

/*
write32 #1 block at +0x00742fbc NV_PRAMIN+0x42fbc of 4 (0x4) elements:
+0x00742fbc: 3f800000 00000000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x281c/4), 0x3f800000);

/*
write32 #1 block at +0x00742ffc NV_PRAMIN+0x42ffc of 12 (0xc) elements:
+0x00742ffc: 40000000 3f800000 3f000000 00000000 40000000 3f800000 00000000 bf800000
+0x0074301c: 00000000 bf800000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x285c/4)+0, 0x40000000);
	INSTANCE_WR(ctx, (0x285c/4)+1, 0x3f800000);
	INSTANCE_WR(ctx, (0x285c/4)+2, 0x3f000000);
	INSTANCE_WR(ctx, (0x285c/4)+4, 0x40000000);
	INSTANCE_WR(ctx, (0x285c/4)+5, 0x3f800000);
	INSTANCE_WR(ctx, (0x285c/4)+7, 0xbf800000);
	INSTANCE_WR(ctx, (0x285c/4)+9, 0xbf800000);

/*
write32 #1 block at +0x00742fcc NV_PRAMIN+0x42fcc of 4 (0x4) elements:
+0x00742fcc: 00000000 3f800000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x282c/4)+1, 0x3f800000);

/*
write32 #1 block at +0x0074302c NV_PRAMIN+0x4302c of 4 (0x4) elements:
+0x0074302c: 00000000 00000000 00000000 00000000
write32 #1 block at +0x00743c9c NV_PRAMIN+0x43c9c of 4 (0x4) elements:
+0x00743c9c: 00000000 00000000 00000000 00000000
write32 #1 block at +0x00743c3c NV_PRAMIN+0x43c3c of 8 (0x8) elements:
+0x00743c3c: 00000000 00000000 000fe000 00000000 00000000 00000000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x349c/4)+2, 0x000fe000);

/*
write32 #1 block at +0x00743c6c NV_PRAMIN+0x43c6c of 4 (0x4) elements:
+0x00743c6c: 00000000 00000000 00000000 00000000
write32 #1 block at +0x00743ccc NV_PRAMIN+0x43ccc of 4 (0x4) elements:
+0x00743ccc: 00000000 000003f8 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x352c/4)+1, 0x000003f8);

/* write32 #1 NV_PRAMIN+0x43ce0 <- 0x002fe000 */
	INSTANCE_WR(ctx, 0x3540/4, 0x002fe000);

/*
write32 #1 block at +0x00743cfc NV_PRAMIN+0x43cfc of 8 (0x8) elements:
+0x00743cfc: 001c527c 001c527c 001c527c 001c527c 001c527c 001c527c 001c527c 001c527c
*/
	for (i = 0; i < 8; ++i)
		INSTANCE_WR(ctx, (0x355c/4)+i, 0x001c527c);
}

static void nv2a_graph_context_init(struct drm_device *dev,
                                    struct nouveau_gpuobj *ctx)
{
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	int i;

	INSTANCE_WR(ctx, 0x33c/4, 0xffff0000);
	for(i = 0x3a0; i< 0x3a8; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0fff0000);
	INSTANCE_WR(ctx, 0x47c/4, 0x00000101);
	INSTANCE_WR(ctx, 0x490/4, 0x00000111);
	INSTANCE_WR(ctx, 0x4a8/4, 0x44400000);
	for(i = 0x4d4; i< 0x4e4; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00030303);
	for(i = 0x4f4; i< 0x504; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00080000);
	for(i = 0x50c; i< 0x51c; i += 4)
		INSTANCE_WR(ctx, i/4, 0x01012000);
	for(i = 0x51c; i< 0x52c; i += 4)
		INSTANCE_WR(ctx, i/4, 0x000105b8);
	for(i = 0x52c; i< 0x53c; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00080008);
	for(i = 0x55c; i< 0x59c; i += 4)
		INSTANCE_WR(ctx, i/4, 0x07ff0000);
	INSTANCE_WR(ctx, 0x5a4/4, 0x4b7fffff);
	INSTANCE_WR(ctx, 0x5fc/4, 0x00000001);
	INSTANCE_WR(ctx, 0x604/4, 0x00004000);
	INSTANCE_WR(ctx, 0x610/4, 0x00000001);
	INSTANCE_WR(ctx, 0x618/4, 0x00040000);
	INSTANCE_WR(ctx, 0x61c/4, 0x00010000);

	for (i=0x1a9c; i <= 0x22fc/4; i += 32) {
		INSTANCE_WR(ctx, i/4    , 0x10700ff9);
		INSTANCE_WR(ctx, i/4 + 1, 0x0436086c);
		INSTANCE_WR(ctx, i/4 + 2, 0x000c001b);
	}

	INSTANCE_WR(ctx, 0x269c/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x26b0/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x26dc/4, 0x40000000);
	INSTANCE_WR(ctx, 0x26e0/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x26e4/4, 0x3f000000);
	INSTANCE_WR(ctx, 0x26ec/4, 0x40000000);
	INSTANCE_WR(ctx, 0x26f0/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x26f8/4, 0xbf800000);
	INSTANCE_WR(ctx, 0x2700/4, 0xbf800000);
	INSTANCE_WR(ctx, 0x3024/4, 0x000fe000);
	INSTANCE_WR(ctx, 0x30a0/4, 0x000003f8);
	INSTANCE_WR(ctx, 0x33fc/4, 0x002fe000);
	for(i = 0x341c; i< 0x343c; i += 4)
		INSTANCE_WR(ctx, i/4, 0x001c527c);
}

static void nv25_graph_context_init(struct drm_device *dev,
                                    struct nouveau_gpuobj *ctx)
{
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	int i;
/*
write32 #1 block at +0x00740a7c NV_PRAMIN.GRCTX0+0x35c of 173 (0xad) elements:
+0x00740a7c: ffff0000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740a9c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740abc: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740adc: 00000000 0fff0000 0fff0000 00000000 00000000 00000000 00000000 00000000
+0x00740afc: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740b1c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740b3c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740b5c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000

+0x00740b7c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740b9c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740bbc: 00000101 00000000 00000000 00000000 00000000 00000111 00000000 00000000
+0x00740bdc: 00000000 00000000 00000000 00000080 ffff0000 00000001 00000000 00000000
+0x00740bfc: 00000000 00000000 44400000 00000000 00000000 00000000 00000000 00000000
+0x00740c1c: 4b800000 00000000 00000000 00000000 00000000 00030303 00030303 00030303
+0x00740c3c: 00030303 00000000 00000000 00000000 00000000 00080000 00080000 00080000
+0x00740c5c: 00080000 00000000 00000000 01012000 01012000 01012000 01012000 000105b8

+0x00740c7c: 000105b8 000105b8 000105b8 00080008 00080008 00080008 00080008 00000000
+0x00740c9c: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 07ff0000
+0x00740cbc: 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000
+0x00740cdc: 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 07ff0000 00000000
+0x00740cfc: 00000000 4b7fffff 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740d1c: 00000000 00000000 00000000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x35c/4)+0, 0xffff0000);
	INSTANCE_WR(ctx, (0x35c/4)+25, 0x0fff0000);
	INSTANCE_WR(ctx, (0x35c/4)+26, 0x0fff0000);
	INSTANCE_WR(ctx, (0x35c/4)+80, 0x00000101);
	INSTANCE_WR(ctx, (0x35c/4)+85, 0x00000111);
	INSTANCE_WR(ctx, (0x35c/4)+91, 0x00000080);
	INSTANCE_WR(ctx, (0x35c/4)+92, 0xffff0000);
	INSTANCE_WR(ctx, (0x35c/4)+93, 0x00000001);
	INSTANCE_WR(ctx, (0x35c/4)+98, 0x44400000);
	INSTANCE_WR(ctx, (0x35c/4)+104, 0x4b800000);
	INSTANCE_WR(ctx, (0x35c/4)+109, 0x00030303);
	INSTANCE_WR(ctx, (0x35c/4)+110, 0x00030303);
	INSTANCE_WR(ctx, (0x35c/4)+111, 0x00030303);
	INSTANCE_WR(ctx, (0x35c/4)+112, 0x00030303);
	INSTANCE_WR(ctx, (0x35c/4)+117, 0x00080000);
	INSTANCE_WR(ctx, (0x35c/4)+118, 0x00080000);
	INSTANCE_WR(ctx, (0x35c/4)+119, 0x00080000);
	INSTANCE_WR(ctx, (0x35c/4)+120, 0x00080000);
	INSTANCE_WR(ctx, (0x35c/4)+123, 0x01012000);
	INSTANCE_WR(ctx, (0x35c/4)+124, 0x01012000);
	INSTANCE_WR(ctx, (0x35c/4)+125, 0x01012000);
	INSTANCE_WR(ctx, (0x35c/4)+126, 0x01012000);
	INSTANCE_WR(ctx, (0x35c/4)+127, 0x000105b8);
	INSTANCE_WR(ctx, (0x35c/4)+128, 0x000105b8);
	INSTANCE_WR(ctx, (0x35c/4)+129, 0x000105b8);
	INSTANCE_WR(ctx, (0x35c/4)+130, 0x000105b8);
	INSTANCE_WR(ctx, (0x35c/4)+131, 0x00080008);
	INSTANCE_WR(ctx, (0x35c/4)+132, 0x00080008);
	INSTANCE_WR(ctx, (0x35c/4)+133, 0x00080008);
	INSTANCE_WR(ctx, (0x35c/4)+134, 0x00080008);
	for (i=0; i<16; ++i)
		INSTANCE_WR(ctx, (0x35c/4)+143+i, 0x07ff0000);
	INSTANCE_WR(ctx, (0x35c/4)+161, 0x4b7fffff);

/*
write32 #1 block at +0x00740d34 NV_PRAMIN.GRCTX0+0x614 of 3136 (0xc40) elements:
+0x00740d34: 00000000 00000000 00000000 00000080 30201000 70605040 b0a09080 f0e0d0c0
+0x00740d54: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00740d74: 00000000 00000000 00000000 00000000 00000001 00000000 00004000 00000000
+0x00740d94: 00000000 00000001 00000000 00040000 00010000 00000000 00000000 00000000
+0x00740db4: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
...
+0x00742214: 00000000 00000000 00000000 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742234: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742254: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742274: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
...
+0x00742a34: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742a54: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742a74: 10700ff9 0436086c 000c001b 00000000 10700ff9 0436086c 000c001b 00000000
+0x00742a94: 10700ff9 0436086c 000c001b 00000000 00000000 00000000 00000000 00000000
+0x00742ab4: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
+0x00742ad4: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x614/4)+3, 0x00000080);
	INSTANCE_WR(ctx, (0x614/4)+4, 0x30201000);
	INSTANCE_WR(ctx, (0x614/4)+5, 0x70605040);
	INSTANCE_WR(ctx, (0x614/4)+6, 0xb0a09080);
	INSTANCE_WR(ctx, (0x614/4)+7, 0xf0e0d0c0);
	INSTANCE_WR(ctx, (0x614/4)+20, 0x00000001);
	INSTANCE_WR(ctx, (0x614/4)+22, 0x00004000);
	INSTANCE_WR(ctx, (0x614/4)+25, 0x00000001);
	INSTANCE_WR(ctx, (0x614/4)+27, 0x00040000);
	INSTANCE_WR(ctx, (0x614/4)+28, 0x00010000);
	for (i=0; i < 0x880/4; i+=4) {
		INSTANCE_WR(ctx, (0x1b04/4)+i+0, 0x10700ff9);
		INSTANCE_WR(ctx, (0x1b04/4)+i+1, 0x0436086c);
		INSTANCE_WR(ctx, (0x1b04/4)+i+2, 0x000c001b);
	}

/*
write32 #1 block at +0x00742e24 NV_PRAMIN.GRCTX0+0x2704 of 4 (0x4) elements:
+0x00742e24: 3f800000 00000000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x2704/4), 0x3f800000);

/*
write32 #1 block at +0x00742e64 NV_PRAMIN.GRCTX0+0x2744 of 12 (0xc) elements:
+0x00742e64: 40000000 3f800000 3f000000 00000000 40000000 3f800000 00000000 bf800000
+0x00742e84: 00000000 bf800000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x2744/4)+0, 0x40000000);
	INSTANCE_WR(ctx, (0x2744/4)+1, 0x3f800000);
	INSTANCE_WR(ctx, (0x2744/4)+2, 0x3f000000);
	INSTANCE_WR(ctx, (0x2744/4)+4, 0x40000000);
	INSTANCE_WR(ctx, (0x2744/4)+5, 0x3f800000);
	INSTANCE_WR(ctx, (0x2744/4)+7, 0xbf800000);
	INSTANCE_WR(ctx, (0x2744/4)+9, 0xbf800000);

/*
write32 #1 block at +0x00742e34 NV_PRAMIN.GRCTX0+0x2714 of 4 (0x4) elements:
+0x00742e34: 00000000 3f800000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x2714/4)+1, 0x3f800000);

/*
write32 #1 block at +0x00742e94 NV_PRAMIN.GRCTX0+0x2774 of 4 (0x4) elements:
+0x00742e94: 00000000 00000000 00000000 00000000
write32 #1 block at +0x00743804 NV_PRAMIN.GRCTX0+0x30e4 of 4 (0x4) elements:
+0x00743804: 00000000 00000000 00000000 00000000
write32 #1 block at +0x007437a4 NV_PRAMIN.GRCTX0+0x3084 of 8 (0x8) elements:
+0x007437a4: 00000000 00000000 000fe000 00000000 00000000 00000000 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x3084/4)+2, 0x000fe000);

/*
write32 #1 block at +0x007437d4 NV_PRAMIN.GRCTX0+0x30b4 of 4 (0x4) elements:
+0x007437d4: 00000000 00000000 00000000 00000000
write32 #1 block at +0x00743824 NV_PRAMIN.GRCTX0+0x3104 of 4 (0x4) elements:
+0x00743824: 00000000 000003f8 00000000 00000000
*/
	INSTANCE_WR(ctx, (0x3104/4)+1, 0x000003f8);

/* write32 #1 NV_PRAMIN.GRCTX0+0x3468 <- 0x002fe000 */
	INSTANCE_WR(ctx, 0x3468/4, 0x002fe000);

/*
write32 #1 block at +0x00743ba4 NV_PRAMIN.GRCTX0+0x3484 of 8 (0x8) elements:
+0x00743ba4: 001c527c 001c527c 001c527c 001c527c 001c527c 001c527c 001c527c 001c527c
*/
	for (i=0; i<8; ++i)
		INSTANCE_WR(ctx, (0x3484/4)+i, 0x001c527c);
}

static void nv30_31_graph_context_init(struct drm_device *dev,
                                       struct nouveau_gpuobj *ctx)
{
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	int i;

	INSTANCE_WR(ctx, 0x410/4, 0x00000101);
	INSTANCE_WR(ctx, 0x424/4, 0x00000111);
	INSTANCE_WR(ctx, 0x428/4, 0x00000060);
	INSTANCE_WR(ctx, 0x444/4, 0x00000080);
	INSTANCE_WR(ctx, 0x448/4, 0xffff0000);
	INSTANCE_WR(ctx, 0x44c/4, 0x00000001);
	INSTANCE_WR(ctx, 0x460/4, 0x44400000);
	INSTANCE_WR(ctx, 0x48c/4, 0xffff0000);
	for(i = 0x4e0; i< 0x4e8; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0fff0000);
	INSTANCE_WR(ctx, 0x4ec/4, 0x00011100);
	for(i = 0x508; i< 0x548; i += 4)
		INSTANCE_WR(ctx, i/4, 0x07ff0000);
	INSTANCE_WR(ctx, 0x550/4, 0x4b7fffff);
	INSTANCE_WR(ctx, 0x58c/4, 0x00000080);
	INSTANCE_WR(ctx, 0x590/4, 0x30201000);
	INSTANCE_WR(ctx, 0x594/4, 0x70605040);
	INSTANCE_WR(ctx, 0x598/4, 0xb8a89888);
	INSTANCE_WR(ctx, 0x59c/4, 0xf8e8d8c8);
	INSTANCE_WR(ctx, 0x5b0/4, 0xb0000000);
	for(i = 0x600; i< 0x640; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00010588);
	for(i = 0x640; i< 0x680; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00030303);
	for(i = 0x6c0; i< 0x700; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0008aae4);
	for(i = 0x700; i< 0x740; i += 4)
		INSTANCE_WR(ctx, i/4, 0x01012000);
	for(i = 0x740; i< 0x780; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00080008);
	INSTANCE_WR(ctx, 0x85c/4, 0x00040000);
	INSTANCE_WR(ctx, 0x860/4, 0x00010000);
	for(i = 0x864; i< 0x874; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00040004);
	for(i = 0x1f18; i<= 0x3088 ; i+= 16) {
		INSTANCE_WR(ctx, i/4 + 0, 0x10700ff9);
		INSTANCE_WR(ctx, i/4 + 1, 0x0436086c);
		INSTANCE_WR(ctx, i/4 + 2, 0x000c001b);
	}
	for(i = 0x30b8; i< 0x30c8; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0000ffff);
	INSTANCE_WR(ctx, 0x344c/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x3808/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x381c/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x3848/4, 0x40000000);
	INSTANCE_WR(ctx, 0x384c/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x3850/4, 0x3f000000);
	INSTANCE_WR(ctx, 0x3858/4, 0x40000000);
	INSTANCE_WR(ctx, 0x385c/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x3864/4, 0xbf800000);
	INSTANCE_WR(ctx, 0x386c/4, 0xbf800000);
}

static void nv34_graph_context_init(struct drm_device *dev,
                                    struct nouveau_gpuobj *ctx)
{
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	int i;

	INSTANCE_WR(ctx, 0x40c/4, 0x01000101);
	INSTANCE_WR(ctx, 0x420/4, 0x00000111);
	INSTANCE_WR(ctx, 0x424/4, 0x00000060);
	INSTANCE_WR(ctx, 0x440/4, 0x00000080);
	INSTANCE_WR(ctx, 0x444/4, 0xffff0000);
	INSTANCE_WR(ctx, 0x448/4, 0x00000001);
	INSTANCE_WR(ctx, 0x45c/4, 0x44400000);
	INSTANCE_WR(ctx, 0x480/4, 0xffff0000);
	for(i = 0x4d4; i< 0x4dc; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0fff0000);
	INSTANCE_WR(ctx, 0x4e0/4, 0x00011100);
	for(i = 0x4fc; i< 0x53c; i += 4)
		INSTANCE_WR(ctx, i/4, 0x07ff0000);
	INSTANCE_WR(ctx, 0x544/4, 0x4b7fffff);
	INSTANCE_WR(ctx, 0x57c/4, 0x00000080);
	INSTANCE_WR(ctx, 0x580/4, 0x30201000);
	INSTANCE_WR(ctx, 0x584/4, 0x70605040);
	INSTANCE_WR(ctx, 0x588/4, 0xb8a89888);
	INSTANCE_WR(ctx, 0x58c/4, 0xf8e8d8c8);
	INSTANCE_WR(ctx, 0x5a0/4, 0xb0000000);
	for(i = 0x5f0; i< 0x630; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00010588);
	for(i = 0x630; i< 0x670; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00030303);
	for(i = 0x6b0; i< 0x6f0; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0008aae4);
	for(i = 0x6f0; i< 0x730; i += 4)
		INSTANCE_WR(ctx, i/4, 0x01012000);
	for(i = 0x730; i< 0x770; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00080008);
	INSTANCE_WR(ctx, 0x850/4, 0x00040000);
	INSTANCE_WR(ctx, 0x854/4, 0x00010000);
	for(i = 0x858; i< 0x868; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00040004);
	for(i = 0x15ac; i<= 0x271c ; i+= 16) {
		INSTANCE_WR(ctx, i/4 + 0, 0x10700ff9);
		INSTANCE_WR(ctx, i/4 + 1, 0x0436086c);
		INSTANCE_WR(ctx, i/4 + 2, 0x000c001b);
	}
	for(i = 0x274c; i< 0x275c; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0000ffff);
	INSTANCE_WR(ctx, 0x2ae0/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x2e9c/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x2eb0/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x2edc/4, 0x40000000);
	INSTANCE_WR(ctx, 0x2ee0/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x2ee4/4, 0x3f000000);
	INSTANCE_WR(ctx, 0x2eec/4, 0x40000000);
	INSTANCE_WR(ctx, 0x2ef0/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x2ef8/4, 0xbf800000);
	INSTANCE_WR(ctx, 0x2f00/4, 0xbf800000);
}

static void nv35_36_graph_context_init(struct drm_device *dev,
                                       struct nouveau_gpuobj *ctx)
{
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	int i;

	INSTANCE_WR(ctx, 0x40c/4, 0x00000101);
	INSTANCE_WR(ctx, 0x420/4, 0x00000111);
	INSTANCE_WR(ctx, 0x424/4, 0x00000060);
	INSTANCE_WR(ctx, 0x440/4, 0x00000080);
	INSTANCE_WR(ctx, 0x444/4, 0xffff0000);
	INSTANCE_WR(ctx, 0x448/4, 0x00000001);
	INSTANCE_WR(ctx, 0x45c/4, 0x44400000);
	INSTANCE_WR(ctx, 0x488/4, 0xffff0000);
	for(i = 0x4dc; i< 0x4e4; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0fff0000);
	INSTANCE_WR(ctx, 0x4e8/4, 0x00011100);
	for(i = 0x504; i< 0x544; i += 4)
		INSTANCE_WR(ctx, i/4, 0x07ff0000);
	INSTANCE_WR(ctx, 0x54c/4, 0x4b7fffff);
	INSTANCE_WR(ctx, 0x588/4, 0x00000080);
	INSTANCE_WR(ctx, 0x58c/4, 0x30201000);
	INSTANCE_WR(ctx, 0x590/4, 0x70605040);
	INSTANCE_WR(ctx, 0x594/4, 0xb8a89888);
	INSTANCE_WR(ctx, 0x598/4, 0xf8e8d8c8);
	INSTANCE_WR(ctx, 0x5ac/4, 0xb0000000);
	for(i = 0x604; i< 0x644; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00010588);
	for(i = 0x644; i< 0x684; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00030303);
	for(i = 0x6c4; i< 0x704; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0008aae4);
	for(i = 0x704; i< 0x744; i += 4)
		INSTANCE_WR(ctx, i/4, 0x01012000);
	for(i = 0x744; i< 0x784; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00080008);
	INSTANCE_WR(ctx, 0x860/4, 0x00040000);
	INSTANCE_WR(ctx, 0x864/4, 0x00010000);
	for(i = 0x868; i< 0x878; i += 4)
		INSTANCE_WR(ctx, i/4, 0x00040004);
	for(i = 0x1f1c; i<= 0x308c ; i+= 16) {
		INSTANCE_WR(ctx, i/4 + 0, 0x10700ff9);
		INSTANCE_WR(ctx, i/4 + 1, 0x0436086c);
		INSTANCE_WR(ctx, i/4 + 2, 0x000c001b);
	}
	for(i = 0x30bc; i< 0x30cc; i += 4)
		INSTANCE_WR(ctx, i/4, 0x0000ffff);
	INSTANCE_WR(ctx, 0x3450/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x380c/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x3820/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x384c/4, 0x40000000);
	INSTANCE_WR(ctx, 0x3850/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x3854/4, 0x3f000000);
	INSTANCE_WR(ctx, 0x385c/4, 0x40000000);
	INSTANCE_WR(ctx, 0x3860/4, 0x3f800000);
	INSTANCE_WR(ctx, 0x3868/4, 0xbf800000);
	INSTANCE_WR(ctx, 0x3870/4, 0xbf800000);
}

int nv20_graph_create_context(struct nouveau_channel *chan)
{
	struct drm_device *dev = chan->dev;
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	void (*ctx_init)(struct drm_device *, struct nouveau_gpuobj *);
	unsigned int ctx_size;
	unsigned int idoffs = 0x28/4;
	int ret;

	switch (dev_priv->chipset) {
	case 0x20:
		ctx_size = NV20_GRCTX_SIZE;
		ctx_init = nv20_graph_context_init;
		idoffs = 0;
		break;
	case 0x25:
	case 0x28:
		ctx_size = NV25_GRCTX_SIZE;
		ctx_init = nv25_graph_context_init;
		break;
	case 0x2a:
		ctx_size = NV2A_GRCTX_SIZE;
		ctx_init = nv2a_graph_context_init;
		idoffs = 0;
		break;
	case 0x30:
	case 0x31:
		ctx_size = NV30_31_GRCTX_SIZE;
		ctx_init = nv30_31_graph_context_init;
		break;
	case 0x34:
		ctx_size = NV34_GRCTX_SIZE;
		ctx_init = nv34_graph_context_init;
		break;
	case 0x35:
	case 0x36:
		ctx_size = NV35_36_GRCTX_SIZE;
		ctx_init = nv35_36_graph_context_init;
		break;
	default:
		ctx_size = 0;
		ctx_init = nv35_36_graph_context_init;
		DRM_ERROR("Please contact the devs if you want your NV%x"
		          " card to work\n", dev_priv->chipset);
		return -ENOSYS;
		break;
	}

	if ((ret = nouveau_gpuobj_new_ref(dev, chan, NULL, 0, ctx_size, 16,
					  NVOBJ_FLAG_ZERO_ALLOC,
					  &chan->ramin_grctx)))
		return ret;

	/* Initialise default context values */
	ctx_init(dev, chan->ramin_grctx->gpuobj);

	/* nv20: INSTANCE_WR(chan->ramin_grctx->gpuobj, 10, chan->id<<24); */
	INSTANCE_WR(chan->ramin_grctx->gpuobj, idoffs, (chan->id<<24)|0x1);
	                                                     /* CTX_USER */

	INSTANCE_WR(dev_priv->ctx_table->gpuobj, chan->id,
			chan->ramin_grctx->instance >> 4);

	return 0;
}

void nv20_graph_destroy_context(struct nouveau_channel *chan)
{
	struct drm_device *dev = chan->dev;
	struct drm_nouveau_private *dev_priv = dev->dev_private;

	if (chan->ramin_grctx)
		nouveau_gpuobj_ref_del(dev, &chan->ramin_grctx);

	INSTANCE_WR(dev_priv->ctx_table->gpuobj, chan->id, 0);
}

int nv20_graph_load_context(struct nouveau_channel *chan)
{
	struct drm_device *dev = chan->dev;
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	uint32_t inst;

	if (!chan->ramin_grctx)
		return -EINVAL;
	inst = chan->ramin_grctx->instance >> 4;

	NV_WRITE(NV20_PGRAPH_CHANNEL_CTX_POINTER, inst);
	NV_WRITE(NV20_PGRAPH_CHANNEL_CTX_XFER,
		 NV20_PGRAPH_CHANNEL_CTX_XFER_LOAD);
	NV_WRITE(NV10_PGRAPH_CTX_CONTROL, 0x10010100);

	nouveau_wait_for_idle(dev);
	return 0;
}

int nv20_graph_save_context(struct nouveau_channel *chan)
{
	struct drm_device *dev = chan->dev;
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	uint32_t inst;

	if (!chan->ramin_grctx)
		return -EINVAL;
	inst = chan->ramin_grctx->instance >> 4;

	NV_WRITE(NV20_PGRAPH_CHANNEL_CTX_POINTER, inst);
	NV_WRITE(NV20_PGRAPH_CHANNEL_CTX_XFER,
		 NV20_PGRAPH_CHANNEL_CTX_XFER_SAVE);

	nouveau_wait_for_idle(dev);
	return 0;
}

static void nv20_graph_rdi(struct drm_device *dev) {
	struct drm_nouveau_private *dev_priv = dev->dev_private;
	int i, writecount = 32;
	uint32_t rdi_index = 0x2c80000;

	if (dev_priv->chipset == 0x20) {
		rdi_index = 0x3d0000;
		writecount = 15;
	}

	NV_WRITE(NV10_PGRAPH_RDI_INDEX, rdi_index);
	for (i = 0; i < writecount; i++)
		NV_WRITE(NV10_PGRAPH_RDI_DATA, 0);

	nouveau_wait_for_idle(dev);
}

int nv20_graph_init(struct drm_device *dev) {
	struct drm_nouveau_private *dev_priv =
		(struct drm_nouveau_private *)dev->dev_private;
	uint32_t tmp, vramsz;
	int ret, i;

	NV_WRITE(NV03_PMC_ENABLE, NV_READ(NV03_PMC_ENABLE) &
			~NV_PMC_ENABLE_PGRAPH);
	NV_WRITE(NV03_PMC_ENABLE, NV_READ(NV03_PMC_ENABLE) |
			 NV_PMC_ENABLE_PGRAPH);

	/* Create Context Pointer Table */
	dev_priv->ctx_table_size = 32 * 4;
	if ((ret = nouveau_gpuobj_new_ref(dev, NULL, NULL, 0,
					  dev_priv->ctx_table_size, 16,
					  NVOBJ_FLAG_ZERO_ALLOC,
					  &dev_priv->ctx_table)))
		return ret;

	NV_WRITE(NV20_PGRAPH_CHANNEL_CTX_TABLE,
		 dev_priv->ctx_table->instance >> 4);

	nv20_graph_rdi(dev);

	NV_WRITE(NV03_PGRAPH_INTR   , 0xFFFFFFFF);
	NV_WRITE(NV03_PGRAPH_INTR_EN, 0xFFFFFFFF);

	NV_WRITE(NV04_PGRAPH_DEBUG_0, 0xFFFFFFFF);
	NV_WRITE(NV04_PGRAPH_DEBUG_0, 0x00000000);
	NV_WRITE(NV04_PGRAPH_DEBUG_1, 0x00118700);
	NV_WRITE(NV04_PGRAPH_DEBUG_3, 0xF3CE0475); /* 0x4 = auto ctx switch */
	NV_WRITE(NV10_PGRAPH_DEBUG_4, 0x00000000);
	NV_WRITE(0x40009C           , 0x00000040);

	if (dev_priv->chipset >= 0x25) {
		NV_WRITE(0x400890, 0x00080000);
		NV_WRITE(0x400610, 0x304B1FB6);
		NV_WRITE(0x400B80, 0x18B82880);
		NV_WRITE(0x400B84, 0x44000000);
		NV_WRITE(0x400098, 0x40000080);
		NV_WRITE(0x400B88, 0x000000ff);
	} else {
		NV_WRITE(0x400880, 0x00080000); /* 0x0008c7df */
		NV_WRITE(0x400094, 0x00000005);
		NV_WRITE(0x400B80, 0x45CAA208); /* 0x45eae20e */
		NV_WRITE(0x400B84, 0x24000000);
		NV_WRITE(0x400098, 0x00000040);
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00E00038);
		NV_WRITE(NV10_PGRAPH_RDI_DATA , 0x00000030);
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00E10038);
		NV_WRITE(NV10_PGRAPH_RDI_DATA , 0x00000030);
	}

	/* copy tile info from PFB */
	for (i = 0; i < NV10_PFB_TILE__SIZE; i++) {
		NV_WRITE(0x00400904 + i*0x10, NV_READ(NV10_PFB_TLIMIT(i)));
			/* which is NV40_PGRAPH_TLIMIT0(i) ?? */
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0030+i*4);
		NV_WRITE(NV10_PGRAPH_RDI_DATA, NV_READ(NV10_PFB_TLIMIT(i)));
		NV_WRITE(0x00400908 + i*0x10, NV_READ(NV10_PFB_TSIZE(i)));
			/* which is NV40_PGRAPH_TSIZE0(i) ?? */
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0050+i*4);
		NV_WRITE(NV10_PGRAPH_RDI_DATA, NV_READ(NV10_PFB_TSIZE(i)));
		NV_WRITE(0x00400900 + i*0x10, NV_READ(NV10_PFB_TILE(i)));
			/* which is NV40_PGRAPH_TILE0(i) ?? */
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0010+i*4);
		NV_WRITE(NV10_PGRAPH_RDI_DATA, NV_READ(NV10_PFB_TILE(i)));
	}
	for (i = 0; i < 8; i++) {
		NV_WRITE(0x400980+i*4, NV_READ(0x100300+i*4));
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0090+i*4);
		NV_WRITE(NV10_PGRAPH_RDI_DATA, NV_READ(0x100300+i*4));
	}
	NV_WRITE(0x4009a0, NV_READ(0x100324));
	NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA000C);
	NV_WRITE(NV10_PGRAPH_RDI_DATA, NV_READ(0x100324));

	NV_WRITE(NV10_PGRAPH_CTX_CONTROL, 0x10000100);
	NV_WRITE(NV10_PGRAPH_STATE      , 0xFFFFFFFF);
	NV_WRITE(NV04_PGRAPH_FIFO       , 0x00000001);

	tmp = NV_READ(NV10_PGRAPH_SURFACE) & 0x0007ff00;
	NV_WRITE(NV10_PGRAPH_SURFACE, tmp);
	tmp = NV_READ(NV10_PGRAPH_SURFACE) | 0x00020100;
	NV_WRITE(NV10_PGRAPH_SURFACE, tmp);

	/* begin RAM config */
	vramsz = drm_get_resource_len(dev, 0) - 1;
	NV_WRITE(0x4009A4, NV_READ(NV04_PFB_CFG0));
	NV_WRITE(0x4009A8, NV_READ(NV04_PFB_CFG1));
	NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0000);
	NV_WRITE(NV10_PGRAPH_RDI_DATA , NV_READ(NV04_PFB_CFG0));
	NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0004);
	NV_WRITE(NV10_PGRAPH_RDI_DATA , NV_READ(NV04_PFB_CFG1));
	NV_WRITE(0x400820, 0);
	NV_WRITE(0x400824, 0);
	NV_WRITE(0x400864, vramsz-1);
	NV_WRITE(0x400868, vramsz-1);

	/* interesting.. the below overwrites some of the tile setup above.. */
	NV_WRITE(0x400B20, 0x00000000);
	NV_WRITE(0x400B04, 0xFFFFFFFF);

	NV_WRITE(NV03_PGRAPH_ABS_UCLIP_XMIN, 0);
	NV_WRITE(NV03_PGRAPH_ABS_UCLIP_YMIN, 0);
	NV_WRITE(NV03_PGRAPH_ABS_UCLIP_XMAX, 0x7fff);
	NV_WRITE(NV03_PGRAPH_ABS_UCLIP_YMAX, 0x7fff);

	return 0;
}

void nv20_graph_takedown(struct drm_device *dev)
{
	struct drm_nouveau_private *dev_priv = dev->dev_private;

	nouveau_gpuobj_ref_del(dev, &dev_priv->ctx_table);
}

int nv30_graph_init(struct drm_device *dev)
{
	struct drm_nouveau_private *dev_priv = dev->dev_private;
//	uint32_t vramsz, tmp;
	int ret, i;

	NV_WRITE(NV03_PMC_ENABLE, NV_READ(NV03_PMC_ENABLE) &
			~NV_PMC_ENABLE_PGRAPH);
	NV_WRITE(NV03_PMC_ENABLE, NV_READ(NV03_PMC_ENABLE) |
			 NV_PMC_ENABLE_PGRAPH);

	/* Create Context Pointer Table */
	dev_priv->ctx_table_size = 32 * 4;
	if ((ret = nouveau_gpuobj_new_ref(dev, NULL, NULL, 0,
					  dev_priv->ctx_table_size, 16,
					  NVOBJ_FLAG_ZERO_ALLOC,
					  &dev_priv->ctx_table)))
		return ret;

	NV_WRITE(NV20_PGRAPH_CHANNEL_CTX_TABLE,
			dev_priv->ctx_table->instance >> 4);

	NV_WRITE(NV03_PGRAPH_INTR   , 0xFFFFFFFF);
	NV_WRITE(NV03_PGRAPH_INTR_EN, 0xFFFFFFFF);

	NV_WRITE(NV04_PGRAPH_DEBUG_0, 0xFFFFFFFF);
	NV_WRITE(NV04_PGRAPH_DEBUG_0, 0x00000000);
	NV_WRITE(NV04_PGRAPH_DEBUG_1, 0x401287c0);
	NV_WRITE(0x400890, 0x01b463ff);
	NV_WRITE(NV04_PGRAPH_DEBUG_3, 0xf2de0475);
	NV_WRITE(NV10_PGRAPH_DEBUG_4, 0x00008000);
	NV_WRITE(NV04_PGRAPH_LIMIT_VIOL_PIX, 0xf04bdff6);
	NV_WRITE(0x400B80, 0x1003d888);
	NV_WRITE(0x400B84, 0x0c000000);
	NV_WRITE(0x400098, 0x00000000);
	NV_WRITE(0x40009C, 0x0005ad00);
	NV_WRITE(0x400B88, 0x62ff00ff); // suspiciously like PGRAPH_DEBUG_2
	NV_WRITE(0x4000a0, 0x00000000);
	NV_WRITE(0x4000a4, 0x00000008);
	NV_WRITE(0x4008a8, 0xb784a400);
	NV_WRITE(0x400ba0, 0x002f8685);
	NV_WRITE(0x400ba4, 0x00231f3f);
	NV_WRITE(0x4008a4, 0x40000020);

	if (dev_priv->chipset == 0x34) {
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0004);
		NV_WRITE(NV10_PGRAPH_RDI_DATA , 0x00200201);
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0008);
		NV_WRITE(NV10_PGRAPH_RDI_DATA , 0x00000008);
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00EA0000);
		NV_WRITE(NV10_PGRAPH_RDI_DATA , 0x00000032);
		NV_WRITE(NV10_PGRAPH_RDI_INDEX, 0x00E00004);
		NV_WRITE(NV10_PGRAPH_RDI_DATA , 0x00000002);
	}

	NV_WRITE(0x4000c0, 0x00000016);

	/* copy tile info from PFB */
	for (i = 0; i < NV10_PFB_TILE__SIZE; i++) {
		NV_WRITE(0x00400904 + i*0x10, NV_READ(NV10_PFB_TLIMIT(i)));
			/* which is NV40_PGRAPH_TLIMIT0(i) ?? */
		NV_WRITE(0x00400908 + i*0x10, NV_READ(NV10_PFB_TSIZE(i)));
			/* which is NV40_PGRAPH_TSIZE0(i) ?? */
		NV_WRITE(0x00400900 + i*0x10, NV_READ(NV10_PFB_TILE(i)));
			/* which is NV40_PGRAPH_TILE0(i) ?? */
	}

	NV_WRITE(NV10_PGRAPH_CTX_CONTROL, 0x10000100);
	NV_WRITE(NV10_PGRAPH_STATE      , 0xFFFFFFFF);
	NV_WRITE(0x0040075c             , 0x00000001);
	NV_WRITE(NV04_PGRAPH_FIFO       , 0x00000001);

	/* begin RAM config */
//	vramsz = drm_get_resource_len(dev, 0) - 1;
	NV_WRITE(0x4009A4, NV_READ(NV04_PFB_CFG0));
	NV_WRITE(0x4009A8, NV_READ(NV04_PFB_CFG1));
	if (dev_priv->chipset != 0x34) {
		NV_WRITE(0x400750, 0x00EA0000);
		NV_WRITE(0x400754, NV_READ(NV04_PFB_CFG0));
		NV_WRITE(0x400750, 0x00EA0004);
		NV_WRITE(0x400754, NV_READ(NV04_PFB_CFG1));
	}

#if 0
	NV_WRITE(0x400820, 0);
	NV_WRITE(0x400824, 0);
	NV_WRITE(0x400864, vramsz-1);
	NV_WRITE(0x400868, vramsz-1);

	NV_WRITE(0x400B20, 0x00000000);
	NV_WRITE(0x400B04, 0xFFFFFFFF);

	/* per-context state, doesn't belong here */
	tmp = NV_READ(NV10_PGRAPH_SURFACE) & 0x0007ff00;
	NV_WRITE(NV10_PGRAPH_SURFACE, tmp);
	tmp = NV_READ(NV10_PGRAPH_SURFACE) | 0x00020100;
	NV_WRITE(NV10_PGRAPH_SURFACE, tmp);

	NV_WRITE(NV03_PGRAPH_ABS_UCLIP_XMIN, 0);
	NV_WRITE(NV03_PGRAPH_ABS_UCLIP_YMIN, 0);
	NV_WRITE(NV03_PGRAPH_ABS_UCLIP_XMAX, 0x7fff);
	NV_WRITE(NV03_PGRAPH_ABS_UCLIP_YMAX, 0x7fff);
#endif

	return 0;
}