/***************************************************************
                             rcp.c
                               
Contains a bunch of display lists to help us initialize the
RCP
***************************************************************/

#include <ultra64.h>
#include "debug.h"
#include "osconfig.h"
#include "graphics.h"


/*********************************
             Globals
*********************************/

// Display lists
Gfx  g_displists[FRAMEBUFF_COUNT][DISPLIST_SIZE];
Gfx* g_displistp;


/*********************************
            Viewports
*********************************/

static const Vp s_viewport_sd = {
    SCREEN_WIDTH_SD*2, SCREEN_HEIGHT_SD*2, G_MAXZ/2, 0,	// Scale 
    SCREEN_WIDTH_SD*2, SCREEN_HEIGHT_SD*2, G_MAXZ/2, 0,	// Translation
};

static const Vp s_viewport_hd = {
    SCREEN_WIDTH_HD*2, SCREEN_HEIGHT_HD*2, G_MAXZ/2, 0,	// Scale 
    SCREEN_WIDTH_HD*2, SCREEN_HEIGHT_HD*2, G_MAXZ/2, 0,	// Translation
};


/*********************************
               RSP
*********************************/

static const Gfx s_dl_rspinit_sd[] = {
    gsSPViewport(&s_viewport_sd),
    gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
                          G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD),
    gsSPTexture(0, 0, 0, 0, G_OFF),
    gsSPEndDisplayList(),
};

static const Gfx s_dl_rspinit_hd[] = {
    gsSPViewport(&s_viewport_hd),
    gsSPClearGeometryMode(G_SHADE | G_SHADING_SMOOTH | G_CULL_BOTH |
                          G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                          G_TEXTURE_GEN_LINEAR | G_LOD),
    gsSPTexture(0, 0, 0, 0, G_OFF),
    gsSPEndDisplayList(),
};


/*********************************
               RDP
*********************************/

static const Gfx s_dl_rdpinit_sd[] = {
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH_SD, SCREEN_HEIGHT_SD),
    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
    gsDPSetColorDither(G_CD_DISABLE),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

static const Gfx s_dl_rdpinit_hd[] = {
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH_HD, SCREEN_HEIGHT_HD),
    gsDPSetCombineKey(G_CK_NONE),
    gsDPSetAlphaCompare(G_AC_NONE),
    gsDPSetRenderMode(G_RM_NOOP, G_RM_NOOP2),
    gsDPSetColorDither(G_CD_DISABLE),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};


/*==============================
    rcp_initialize_sd
    Initializes the RCP for drawing in standard definition
    @param A pointer to the render task struct to use
==============================*/

void rcp_initialize_sd(RenderTask* task)
{
    u8 l_clearcolor = task->color;
    g_displistp = task->displistp;
    
    // Set the segment register to the first segment
    // The addresses passed to the RCP must be in segment addresses
    // The CPU uses virtual addresses, this will convert accordingly
    gSPSegment(g_displistp++, 0, 0x0);
    
    // Set the framebuffer that the RDP will draw onto
    gDPSetColorImage(g_displistp++, G_IM_FMT_RGBA, task->bufferdepth, SCREEN_WIDTH_SD, OS_K0_TO_PHYSICAL(task->framebuffer));
    
    // Initialize the RSP and RDP with our initialization display lists
    gSPDisplayList(g_displistp++, s_dl_rdpinit_sd);
    gSPDisplayList(g_displistp++, s_dl_rspinit_sd);
    
    // Clear the framebuffer
    gDPPipeSync(g_displistp++);
    gDPSetCycleType(g_displistp++, G_CYC_FILL);
    gDPSetFillColor(g_displistp++, GPACK_RGBA5551(l_clearcolor, l_clearcolor, l_clearcolor, 1) << 16 | GPACK_RGBA5551(l_clearcolor, l_clearcolor, l_clearcolor, 1));
    gDPFillRectangle(g_displistp++, 0, 0, SCREEN_WIDTH_SD-1, SCREEN_HEIGHT_SD-1);
    gDPPipeSync(g_displistp++);
}


/*==============================
    rcp_initialize_sd
    Initializes the RCP for drawing in high definition
    @param A pointer to the render task struct to use
==============================*/

void rcp_initialize_hd(RenderTask* task)
{
    u8 l_clearcolor = task->color;
    g_displistp = task->displistp;
    
    // Set the segment register to the first segment
    // The addresses passed to the RCP must be in segment addresses
    // The CPU uses virtual addresses, this will convert accordingly
    gSPSegment(g_displistp++, 0, 0x0);
    
    // Set the framebuffer that the RDP will draw onto
    gDPSetColorImage(g_displistp++, G_IM_FMT_RGBA, task->bufferdepth, SCREEN_WIDTH_HD, OS_K0_TO_PHYSICAL(task->framebuffer));
    
    // Initialize the RSP and RDP with our initialization display lists
    gSPDisplayList(g_displistp++, s_dl_rdpinit_hd);
    gSPDisplayList(g_displistp++, s_dl_rspinit_hd);
    
    // Clear the framebuffer
    gDPPipeSync(g_displistp++);
    gDPSetCycleType(g_displistp++, G_CYC_FILL);
    gDPSetFillColor(g_displistp++, GPACK_RGBA5551(l_clearcolor, l_clearcolor, l_clearcolor, 1) << 16 | GPACK_RGBA5551(l_clearcolor, l_clearcolor, l_clearcolor, 1));
    gDPFillRectangle(g_displistp++, 0, 0, SCREEN_WIDTH_HD-1, SCREEN_HEIGHT_HD-1);
    gDPPipeSync(g_displistp++);
}


/*==============================
    rcp_finish
    Finish a display list and writebacks data
    @param A pointer to the render task struct to use
==============================*/

void rcp_finish(RenderTask* task)
{
    // Finish the display list
    gDPFullSync(g_displistp++);
    gSPEndDisplayList(g_displistp++);
    
    // Ensure we haven't gone over the display list limit
    debug_assert((g_displistp - task->displistp) < DISPLIST_SIZE);
    
    // Writeback cache lines so that the RCP can read the up-to-date data
    osWritebackDCacheAll();
}