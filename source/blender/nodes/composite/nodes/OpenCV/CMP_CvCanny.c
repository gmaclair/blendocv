/**
 * $Id: CMP_hueSatVal.c 14749 2008-05-08 19:57:10Z blendix $
 *
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version. 
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * The Original Code is Copyright (C) 2006 Blender Foundation.
 * All rights reserved.
 *
 * The Original Code is: all of this file.
 *
 * Contributor(s): none yet.
 *
 * ***** END GPL LICENSE BLOCK *****
 */

#include "../../node_composite_util.h"
#include "../../BOCV_util.h"
#include <stdio.h>
//#include <cxerror.h>


static bNodeSocketTemplate cmp_node_cvCanny_in[]= {
	{	SOCK_RGBA, 1, "Image",			1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},	
	{	SOCK_FLOAT, 1, "Threshold 1",			50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 255.0f},
        {	SOCK_FLOAT, 1, "Threshold 2",			50.0f, 0.0f, 0.0f, 0.0f, 0.0f, 255.0f},
	{	-1, 0, ""	}
};
static bNodeSocketTemplate cmp_node_cvCanny_out[]= {
	{	SOCK_RGBA, 0, "Image",			0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f},
	{	-1, 0, ""	}
};

static void node_composit_exec_cvCanny(void *data, bNode *node, bNodeStack **in, bNodeStack **out)
{
	int w,h,aperture;
	float thresh1, thresh2;
        IplImage *img, *edge, *s8boutput, *s8binput;
        CompBuf* dst_buf;

	if(out[0]->hasoutput==0) return;
	if(in[0]->data){
		
            img = BOCV_IplImage_attach(in[0]->data);
            thresh1= in[1]->vec[0];
            thresh2= in[2]->vec[0];
            w=img->width;
            h=img->height;

            switch(node->custom1){
                    case 0:
                      aperture=1;
                      break;
                    case 1:
                      aperture=3;
                      break;
                    case 2:
                      aperture=5;
                      break;
                    case 3:
                      aperture=7;
                      break;

            }
            //Only accept 1 channel image
            if(img->nChannels!=1){
                node->error= 1;
                return;
            }
            
            dst_buf=alloc_compbuf(w,h,CB_VAL,1);
            edge = BOCV_IplImage_attach(dst_buf);
            
            //Convert 8-bit
            s8binput = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 1);
            cvConvertScale(img, s8binput,255,0);
            
            s8boutput = cvCreateImage(cvSize(w,h), IPL_DEPTH_8U, 1);
            
            cvCanny(s8binput, s8boutput, thresh1, thresh2, aperture);

            //Convert 8bit output to float because blender only support float sockets
            cvConvertScale(s8boutput, edge,1.0/255.0,0);
            
            out[0]->data= dst_buf;
            
            generate_preview(data, node, dst_buf);
            
            BOCV_IplImage_detach(s8binput);
            BOCV_IplImage_detach(s8boutput);
            BOCV_IplImage_detach(edge);
            BOCV_IplImage_detach(img);
	}
}

static void node_composit_init_cvcanny(bNodeTree *UNUSED(ntree), bNode* node, bNodeTemplate *UNUSED(ntemp))
{
	node->custom1= 1;
}

void register_node_type_cmp_cvcanny(ListBase *lb)
{
	static bNodeType ntype;
	
	node_type_base(&ntype, CMP_NODE_CVCANNY, "OpenCV - Canny", NODE_CLASS_OCV_IMAGEPROCESS, NODE_PREVIEW|NODE_OPTIONS);
	node_type_socket_templates(&ntype,cmp_node_cvCanny_in, cmp_node_cvCanny_out);
	node_type_size(&ntype, 150, 80, 250);
        node_type_init(&ntype, node_composit_init_cvcanny);
	node_type_exec(&ntype, node_composit_exec_cvCanny);
	
	nodeRegisterType(lb, &ntype);
}


