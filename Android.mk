ifneq ($(TARGET_SIMULATOR),true)

LOCAL_PATH:= $(call my-dir)

BUILD_FOR_HOST:=0

#
# libocr (common)
#

LOCAL_SRC_FILES_:=		\
	ccutil/ambigs.cpp	\
	ccutil/basedir.cpp	\
	ccutil/bits16.cpp	\
	ccutil/boxread.cpp	\
	ccutil/clst.cpp		\
	ccutil/debugwin.cpp	\
	ccutil/elst.cpp		\
	ccutil/elst2.cpp	\
	ccutil/errcode.cpp	\
	ccutil/globaloc.cpp	\
	ccutil/hashfn.cpp	\
	ccutil/mainblk.cpp	\
	ccutil/memblk.cpp	\
	ccutil/memry.cpp	\
	ccutil/mfcpch.cpp	\
	ccutil/ocrshell.cpp	\
	ccutil/scanutils.cpp	\
	ccutil/serialis.cpp	\
	ccutil/strngs.cpp	\
	ccutil/tessdatamanager.cpp	\
	ccutil/tessopt.cpp	\
	ccutil/tordvars.cpp	\
	ccutil/tprintf.cpp	\
	ccutil/unichar.cpp	\
	ccutil/unicharmap.cpp	\
	ccutil/unicharset.cpp	\
	ccutil/varable.cpp	\
	ccutil/ccutil.cpp

LOCAL_SRC_FILES_+=		\
	api/baseapi.cpp

LOCAL_SRC_FILES_+=		\
	viewer/scrollview.cpp	\
	viewer/svmnode.cpp	\
	viewer/svutil.cpp
#	viewer/svpaint.cpp	\

LOCAL_SRC_FILES_+=		\
	cutil/bitvec.cpp	\
	cutil/cutil.cpp		\
	cutil/danerror.cpp	\
	cutil/efio.cpp		\
	cutil/emalloc.cpp	\
	cutil/freelist.cpp	\
	cutil/globals.cpp	\
	cutil/listio.cpp	\
	cutil/oldheap.cpp	\
	cutil/oldlist.cpp	\
	cutil/structures.cpp	\
	cutil/tessarray.cpp	\
	cutil/cutil_class.cpp 

LOCAL_SRC_FILES_+=		\
	image/image.cpp		\
	image/imgbmp.cpp	\
	image/imgio.cpp		\
	image/imgs.cpp		\
	image/imgtiff.cpp	\
	image/bitstrm.cpp	\
	image/svshowim.cpp

LOCAL_SRC_FILES_+=		\
	liblept/adaptmap.c	\
	liblept/affine.c	\
	liblept/affinecompose.c	\
	liblept/arithlow.c	\
	liblept/arrayaccess.c	\
	liblept/bardecode.c	\
	liblept/baseline.c	\
	liblept/bbuffer.c	\
	liblept/bilinear.c	\
	liblept/binarize.c	\
	liblept/binexpand.c	\
	liblept/binexpandlow.c	\
	liblept/binreduce.c	\
	liblept/binreducelow.c	\
	liblept/blend.c		\
	liblept/bmf.c		\
	liblept/bmpio.c		\
	liblept/bmpiostub.c	\
	liblept/boxbasic.c	\
	liblept/boxfunc1.c	\
	liblept/boxfunc2.c	\
	liblept/boxfunc3.c	\
	liblept/ccbord.c	\
	liblept/ccthin.c	\
	liblept/classapp.c	\
	liblept/colorcontent.c	\
	liblept/colormap.c	\
	liblept/colormorph.c	\
	liblept/colorquant1.c	\
	liblept/colorquant2.c	\
	liblept/colorseg.c	\
	liblept/compare.c	\
	liblept/conncomp.c	\
	liblept/convolve.c	\
	liblept/convolvelow.c	\
	liblept/correlscore.c	\
	liblept/dwacomb.2.c	\
	liblept/dwacomblow.2.c	\
	liblept/edge.c		\
	liblept/endiantest.c	\
	liblept/enhance.c	\
	liblept/fhmtauto.c	\
	liblept/fhmtgen.1.c	\
	liblept/fhmtgenlow.1.c	\
	liblept/flipdetect.c	\
	liblept/fliphmtgen.c	\
	liblept/fmorphauto.c	\
	liblept/fmorphgen.1.c	\
	liblept/fmorphgenlow.1.c	\
	liblept/fpix1.c		\
	liblept/fpix2.c		\
	liblept/gifio.c		\
	liblept/gifiostub.c	\
	liblept/gplot.c		\
	liblept/graphics.c	\
	liblept/graymorph.c	\
	liblept/graymorphlow.c	\
	liblept/grayquant.c	\
	liblept/grayquantlow.c	\
	liblept/heap.c		\
	liblept/jbclass.c	\
	liblept/jpegio.c	\
	liblept/jpegiostub.c	\
	liblept/kernel.c	\
	liblept/list.c		\
	liblept/maze.c		\
	liblept/morphapp.c	\
	liblept/morph.c		\
	liblept/morphdwa.c	\
	liblept/morphseq.c	\
	liblept/numabasic.c	\
	liblept/numafunc1.c	\
	liblept/numafunc2.c	\
	liblept/pageseg.c	\
	liblept/paintcmap.c	\
	liblept/parseprotos.c	\
	liblept/partition.c	\
	liblept/pix1.c		\
	liblept/pix2.c		\
	liblept/pix3.c		\
	liblept/pix4.c		\
	liblept/pixabasic.c	\
	liblept/pixacc.c	\
	liblept/pixafunc1.c	\
	liblept/pixafunc2.c	\
	liblept/pixalloc.c	\
	liblept/pixarith.c	\
	liblept/pixconv.c	\
	liblept/pixtiling.c	\
	liblept/pngio.c		\
	liblept/pngiostub.c	\
	liblept/pnmio.c		\
	liblept/pnmiostub.c	\
	liblept/projective.c	\
	liblept/psio.c		\
	liblept/psiostub.c	\
	liblept/ptra.c		\
	liblept/pts.c		\
	liblept/queue.c		\
	liblept/rank.c		\
	liblept/readbarcode.c	\
	liblept/readfile.c	\
	liblept/rop.c		\
	liblept/ropiplow.c	\
	liblept/roplow.c	\
	liblept/rotateam.c	\
	liblept/rotateamlow.c	\
	liblept/rotate.c	\
	liblept/rotateorth.c	\
	liblept/rotateorthlow.c	\
	liblept/rotateshear.c	\
	liblept/runlength.c	\
	liblept/sarray.c	\
	liblept/scale.c		\
	liblept/scalelow.c	\
	liblept/seedfill.c	\
	liblept/seedfilllow.c	\
	liblept/sel1.c		\
	liblept/sel2.c		\
	liblept/selgen.c	\
	liblept/shear.c		\
	liblept/skew.c		\
	liblept/stack.c		\
	liblept/textops.c	\
	liblept/tiffio.c	\
	liblept/tiffiostub.c	\
	liblept/utils.c		\
	liblept/viewfiles.c	\
	liblept/warper.c	\
	liblept/watershed.c	\
	liblept/writefile.c	\
	liblept/zlibmem.c	\
	liblept/zlibmemstub.c	\

LOCAL_SRC_FILES_+=		\
	ccstruct/blobbox.cpp	\
	ccstruct/blobs.cpp	\
	ccstruct/blread.cpp	\
	ccstruct/callcpp.cpp	\
	ccstruct/coutln.cpp	\
	ccstruct/detlinefit.cpp	\
	ccstruct/genblob.cpp	\
	ccstruct/labls.cpp	\
	ccstruct/linlsq.cpp	\
	ccstruct/lmedsq.cpp	\
	ccstruct/mod128.cpp	\
	ccstruct/normalis.cpp	\
	ccstruct/ocrblock.cpp	\
	ccstruct/ocrrow.cpp	\
	ccstruct/otsuthr.cpp	\
	ccstruct/pageres.cpp	\
	ccstruct/pdblock.cpp	\
	ccstruct/points.cpp	\
	ccstruct/polyaprx.cpp	\
	ccstruct/polyblk.cpp	\
	ccstruct/polyblob.cpp	\
	ccstruct/polyvert.cpp	\
	ccstruct/poutline.cpp	\
	ccstruct/quadlsq.cpp	\
	ccstruct/quadratc.cpp	\
	ccstruct/quspline.cpp	\
	ccstruct/ratngs.cpp	\
	ccstruct/rect.cpp	\
	ccstruct/rejctmap.cpp	\
	ccstruct/statistc.cpp	\
	ccstruct/stepblob.cpp	\
	ccstruct/vecfuncs.cpp	\
	ccstruct/werd.cpp	\
	ccstruct/ccstruct.cpp

LOCAL_SRC_FILES_+=		\
	dict/choices.cpp	\
	dict/context.cpp	\
	dict/conversion.cpp	\
	dict/dawg.cpp		\
	dict/dict.cpp		\
	dict/hyphen.cpp		\
	dict/permdawg.cpp	\
	dict/permngram.cpp	\
	dict/permute.cpp	\
	dict/states.cpp		\
	dict/stopper.cpp	\
	dict/trie.cpp

LOCAL_SRC_FILES_+=		\
	classify/adaptive.cpp	\
	classify/adaptmatch.cpp	\
	classify/baseline.cpp	\
	classify/blobclass.cpp	\
	classify/chartoname.cpp	\
	classify/classify.cpp	\
	classify/cluster.cpp	\
	classify/clusttool.cpp	\
	classify/cutoffs.cpp	\
	classify/extract.cpp	\
	classify/featdefs.cpp	\
	classify/flexfx.cpp	\
	classify/float2int.cpp	\
	classify/fpoint.cpp	\
	classify/fxdefs.cpp	\
	classify/hideedge.cpp	\
	classify/intfx.cpp	\
	classify/intmatcher.cpp	\
	classify/intproto.cpp	\
	classify/kdtree.cpp	\
	classify/mf.cpp		\
	classify/mfdefs.cpp	\
	classify/mfoutline.cpp	\
	classify/mfx.cpp	\
	classify/normfeat.cpp	\
	classify/normmatch.cpp	\
	classify/ocrfeatures.cpp\
	classify/outfeat.cpp	\
	classify/picofeat.cpp	\
	classify/protos.cpp	\
	classify/speckle.cpp	\
	classify/xform2d.cpp

LOCAL_SRC_FILES_+=		\
	wordrec/associate.cpp	\
	wordrec/badwords.cpp	\
	wordrec/bestfirst.cpp	\
	wordrec/chop.cpp	\
	wordrec/chopper.cpp	\
	wordrec/closed.cpp	\
	wordrec/drawfx.cpp	\
	wordrec/findseam.cpp	\
	wordrec/gradechop.cpp	\
	wordrec/heuristic.cpp	\
	wordrec/makechop.cpp	\
	wordrec/matchtab.cpp	\
	wordrec/matrix.cpp	\
	wordrec/metrics.cpp	\
	wordrec/mfvars.cpp	\
	wordrec/olutil.cpp	\
	wordrec/outlines.cpp	\
	wordrec/pieces.cpp	\
	wordrec/plotedges.cpp	\
	wordrec/plotseg.cpp	\
	wordrec/render.cpp	\
	wordrec/seam.cpp	\
	wordrec/split.cpp	\
	wordrec/tally.cpp	\
	wordrec/tessinit.cpp	\
	wordrec/tface.cpp	\
	wordrec/wordclass.cpp	\
	wordrec/wordrec.cpp

LOCAL_SRC_FILES_+=		\
	ccmain/tessvars.cpp	\
	ccmain/tstruct.cpp 	\
	ccmain/reject.cpp 	\
	ccmain/callnet.cpp	\
	ccmain/charcut.cpp	\
	ccmain/docqual.cpp	\
	ccmain/paircmp.cpp	\
	ccmain/adaptions.cpp	\
	ccmain/ambigsrecog.cpp	\
	ccmain/applybox.cpp	\
	ccmain/blobcmp.cpp	\
	ccmain/charsample.cpp	\
	ccmain/control.cpp	\
	ccmain/expandblob.cpp	\
	ccmain/fixspace.cpp	\
	ccmain/fixxht.cpp	\
	ccmain/imgscale.cpp	\
	ccmain/matmatch.cpp	\
	ccmain/osdetect.cpp	\
	ccmain/output.cpp	\
	ccmain/pagewalk.cpp	\
	ccmain/pgedit.cpp	\
	ccmain/scaleimg.cpp	\
	ccmain/tessbox.cpp	\
	ccmain/tesseractclass.cpp	\
	ccmain/tfacepp.cpp	\
	ccmain/thresholder.cpp	\
	ccmain/varabled.cpp	\
	ccmain/werdit.cpp	\
	ccmain/tessedit.cpp
#	ccmain/tessembedded.cpp	\

LOCAL_SRC_FILES_+=		\
	textord/alignedblob.cpp	\
	textord/bbgrid.cpp	\
	textord/blkocc.cpp	\
	textord/colfind.cpp	\
	textord/colpartition.cpp	\
	textord/colpartitionset.cpp	\
	textord/drawedg.cpp	\
	textord/drawtord.cpp	\
       	textord/edgblob.cpp	\
	textord/edgloop.cpp	\
	textord/fpchop.cpp	\
	textord/gap_map.cpp	\
	textord/imagefind.cpp	\
	textord/linefind.cpp	\
	textord/makerow.cpp	\
	textord/oldbasel.cpp	\
	textord/pagesegmain.cpp	\
	textord/pithsync.cpp	\
	textord/pitsync1.cpp	\
	textord/scanedg.cpp	\
	textord/sortflts.cpp	\
	textord/strokewidth.cpp	\
	textord/tabfind.cpp	\
	textord/tablefind.cpp	\
	textord/tabvector.cpp	\
	textord/topitch.cpp	\
	textord/tordmain.cpp	\
	textord/tospace.cpp	\
	textord/tovars.cpp	\
	textord/underlin.cpp	\
	textord/wordseg.cpp     \
	textord/workingpartset.cpp

LOCAL_C_INCLUDES_+=		\
	$(LOCAL_PATH)/ccmain		\
	$(LOCAL_PATH)/api	\
	$(LOCAL_PATH)/ccstruct 		\
	$(LOCAL_PATH)/cstruct 		\
	$(LOCAL_PATH)/cutil		\
	$(LOCAL_PATH)/ccutil 		\
	$(LOCAL_PATH)/ccmain 		\
	$(LOCAL_PATH)/image 		\
	$(LOCAL_PATH)/include	\
	$(LOCAL_PATH)/liblept	\
	$(LOCAL_PATH)/dict 		\
	$(LOCAL_PATH)/classify		\
	$(LOCAL_PATH)/viewer	\
	$(LOCAL_PATH)/wordrec		\
	$(LOCAL_PATH)/textord	\
	external/jpeg

LOCAL_CFLAGS_:=			\
	-DGRAPHICS_DISABLED	\
	-DHAVE_LIBLEPT          \
        -O3
#	-DFST_DISABLED		\
#	-DDISABLE_INTEGER_MATCHING	\
#	-DDISABLE_DOC_DICT	\

LOCAL_STATIC_LIBRARIES_:=	\
	libjpeg

#
# libocr (native)
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=$(LOCAL_SRC_FILES_) 	\
	api/jni.cpp			\
	liblept/_open_memstream.c	\
	liblept/_fopencookie.c		\
	liblept/_fmemopen.c

LOCAL_C_INCLUDES:=$(LOCAL_C_INCLUDES_)
LOCAL_CFLAGS:=$(LOCAL_CFLAGS_)
LOCAL_STATIC_LIBRARIES:=$(LOCAL_STATIC_LIBRARIES_)

LOCAL_SHARED_LIBRARIES:= \
	liblog

LOCAL_MODULE_TAGS := optional

LOCAL_MODULE:= libocr

LOCAL_PRELINK_MODULE:= false

include $(BUILD_SHARED_LIBRARY)

ifeq ($(BUILD_FOR_HOST),1)

#
# libocr (host)
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=$(LOCAL_SRC_FILES_) api/jni.cpp
LOCAL_C_INCLUDES:=$(LOCAL_C_INCLUDES_)
LOCAL_CFLAGS:=$(LOCAL_CFLAGS_)
LOCAL_STATIC_LIBRARIES:=$(LOCAL_STATIC_LIBRARIES_)
LOCAL_SHARED_LIBRARIES:=$(LOCAL_SHARED_LIBRARIES_)

LOCAL_MODULE:= libocr

include $(BUILD_HOST_SHARED_LIBRARY)

#
# tesseract test (host)
#

include $(CLEAR_VARS)

LOCAL_MODULE:= tesseract

LOCAL_SRC_FILES:= \
	api/tesseractmain.cpp

LOCAL_CFLAGS:= \
	-DGRAPHICS_DISABLED	\
	-DHAVE_LIBLEPT		\
	-DFST_DISABLED		\
	-DBUILD_HOST

LOCAL_C_INCLUDES+= \
	$(LOCAL_PATH)/ccutil		\
	$(LOCAL_PATH)/ccstruct	\
	$(LOCAL_PATH)/image		\
	$(LOCAL_PATH)/include	\
	$(LOCAL_PATH)/dict		\
	$(LOCAL_PATH)/classify	\
	$(LOCAL_PATH)/wordrec	\
	$(LOCAL_PATH)/viewer	\
	$(LOCAL_PATH)/cutil		\
	$(LOCAL_PATH)/textord	\
	$(LOCAL_PATH)/api	\
	$(LOCAL_PATH)/ccmain

LOCAL_SHARED_LIBRARIES:= \
	libocr

include $(BUILD_HOST_EXECUTABLE)

#
# simple raw-YUV test (host)
#

include $(CLEAR_VARS)

LOCAL_MODULE:= tesstest 

LOCAL_SRC_FILES:= \
	ccmain/test.cpp

LOCAL_CFLAGS:= \
	-DGRAPHICS_DISABLED	\
	-DHAVE_LIBLEPT		\
	-DFST_DISABLED		\
	-DBUILD_HOST

LOCAL_C_INCLUDES+= \
	$(LOCAL_PATH)/ccutil		\
	$(LOCAL_PATH)/ccstruct	\
	$(LOCAL_PATH)/image		\
	$(LOCAL_PATH)/include	\
	$(LOCAL_PATH)/liblept   \
	$(LOCAL_PATH)/dict		\
	$(LOCAL_PATH)/classify	\
	$(LOCAL_PATH)/wordrec	\
	$(LOCAL_PATH)/viewer	\
	$(LOCAL_PATH)/cutil		\
	$(LOCAL_PATH)/textord	\
	$(LOCAL_PATH)/api	\
	$(LOCAL_PATH)/ccmain

LOCAL_SHARED_LIBRARIES:= \
	libocr

include $(BUILD_HOST_EXECUTABLE)

endif #BUILD_FOR_HOST

endif #TARGET_SIMULATOR
