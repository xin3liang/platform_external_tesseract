LOCAL_PATH:= $(call my-dir)

BUILD_FOR_HOST:=1

#
# libocr (common)
#

local_SRC_FILES:= 		\
	ccutil/basedir.cpp	\
	ccutil/bits16.cpp	\
	ccutil/boxread.cpp	\
	ccutil/clst.cpp		\
	ccutil/debugwin.cpp	\
	ccutil/elst.cpp		\
	ccutil/elst2.cpp	\
	ccutil/errcode.cpp	\
	ccutil/tessopt.cpp	\
	ccutil/globaloc.cpp	\
	ccutil/hashfn.cpp	\
	ccutil/mainblk.cpp	\
	ccutil/memblk.cpp	\
	ccutil/memry.cpp	\
	ccutil/ocrshell.cpp	\
	ccutil/serialis.cpp	\
	ccutil/strngs.cpp	\
	ccutil/cprintf.cpp	\
	ccutil/tprintf.cpp	\
	ccutil/unichar.cpp	\
	ccutil/unicharset.cpp	\
	ccutil/unicharmap.cpp	\
	ccutil/varable.cpp	\
	ccutil/ccutil.cpp

local_SRC_FILES+= 		\
	cutil/tessarray.cpp	\
	cutil/bitvec.cpp	\
	cutil/danerror.cpp	\
	cutil/debug.cpp		\
	cutil/efio.cpp		\
	cutil/emalloc.cpp	\
	cutil/freelist.cpp	\
	cutil/globals.cpp	\
	cutil/listio.cpp	\
	cutil/oldheap.cpp	\
	cutil/oldlist.cpp	\
	cutil/structures.cpp	\
	cutil/tordvars.cpp	\
	cutil/cutil.cpp		\
	cutil/variables.cpp	\
	cutil/cutil_class.cpp 

local_SRC_FILES+= 		\
	image/image.cpp		\
	image/imgbmp.cpp	\
	image/imgio.cpp		\
	image/imgs.cpp		\
	image/imgtiff.cpp	\
	image/bitstrm.cpp	\
	image/svshowim.cpp

local_SRC_FILES+=		\
	ccstruct/blobbox.cpp	\
	ccstruct/blobs.cpp	\
	ccstruct/blread.cpp	\
	ccstruct/callcpp.cpp	\
	ccstruct/ccstruct.cpp	\
	ccstruct/coutln.cpp	\
	ccstruct/genblob.cpp	\
	ccstruct/labls.cpp	\
	ccstruct/linlsq.cpp	\
	ccstruct/lmedsq.cpp	\
	ccstruct/mod128.cpp	\
	ccstruct/normalis.cpp	\
	ccstruct/ocrblock.cpp	\
	ccstruct/ocrrow.cpp	\
	ccstruct/pageblk.cpp	\
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
	ccstruct/rwpoly.cpp	\
	ccstruct/statistc.cpp	\
	ccstruct/stepblob.cpp	\
	ccstruct/txtregn.cpp	\
	ccstruct/vecfuncs.cpp	\
	ccstruct/werd.cpp

local_SRC_FILES+= 		\
	pageseg/pageseg.cpp

local_SRC_FILES+= 		\
	dict/choices.cpp	\
	dict/context.cpp	\
	dict/conversion.cpp	\
	dict/dawg.cpp		\
	dict/dict.cpp		\
	dict/hyphen.cpp		\
	dict/permdawg.cpp	\
	dict/permnum.cpp	\
	dict/permngram.cpp	\
	dict/permute.cpp	\
	dict/states.cpp		\
	dict/stopper.cpp	\
	dict/reduce.cpp		\
	dict/makedawg.cpp	\
	dict/lookdawg.cpp	\
	dict/trie.cpp

local_SRC_FILES+= 		\
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
	classify/sigmenu.cpp	\
	classify/speckle.cpp	\
	classify/xform2d.cpp

local_SRC_FILES+= 		\
	wordrec/associate.cpp	\
	wordrec/badwords.cpp	\
	wordrec/bestfirst.cpp	\
	wordrec/chop.cpp	\
	wordrec/chopper.cpp	\
	wordrec/closed.cpp	\
	wordrec/djmenus.cpp	\
	wordrec/drawfx.cpp	\
	wordrec/findseam.cpp	\
	wordrec/gradechop.cpp	\
	wordrec/heuristic.cpp	\
	wordrec/makechop.cpp	\
	wordrec/matchtab.cpp	\
	wordrec/matrix.cpp	\
	wordrec/metrics.cpp	\
	wordrec/mfvars.cpp	\
	wordrec/msmenus.cpp	\
	wordrec/olutil.cpp	\
	wordrec/outlines.cpp	\
	wordrec/pieces.cpp	\
	wordrec/render.cpp	\
	wordrec/seam.cpp	\
	wordrec/split.cpp	\
	wordrec/tally.cpp	\
	wordrec/tessinit.cpp	\
	wordrec/tface.cpp	\
	wordrec/wordclass.cpp	\
	wordrec/wordrec.cpp
#	wordrec/plotedges.cpp
#	wordrec/plotseg.cpp

local_SRC_FILES+= 		\
	ccmain/tessvars.cpp

local_SRC_FILES+= 		\
	ccmain/tstruct.cpp 	\
	ccmain/reject.cpp 	\
	ccmain/callnet.cpp	\
	ccmain/charcut.cpp	\
	ccmain/docqual.cpp	\
	ccmain/paircmp.cpp	\
	ccmain/adaptions.cpp	\
	ccmain/applybox.cpp	\
	ccmain/baseapi.cpp	\
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
	ccmain/otsuthr.cpp	\
	ccmain/pagewalk.cpp	\
	ccmain/scaleimg.cpp	\
	ccmain/tessbox.cpp	\
	ccmain/tesseractclass.cpp	\
	ccmain/thresholder.cpp	\
	ccmain/tfacepp.cpp	\
	ccmain/varabled.cpp	\
	ccmain/werdit.cpp	\
	ccmain/tessedit.cpp	\
#	ccmain/pgedit.cpp

local_SRC_FILES+= \
	textord/blkocc.cpp	\
       	textord/edgblob.cpp	\
	textord/edgloop.cpp	\
	textord/fpchop.cpp	\
	textord/gap_map.cpp	\
	textord/makerow.cpp	\
	textord/oldbasel.cpp	\
	textord/pithsync.cpp	\
	textord/pitsync1.cpp	\
	textord/scanedg.cpp	\
	textord/sortflts.cpp	\
	textord/topitch.cpp	\
	textord/tordmain.cpp	\
	textord/tospace.cpp	\
	textord/tovars.cpp	\
	textord/underlin.cpp	\
	textord/wordseg.cpp
#	textord/drawtord.cpp
#	textord/drawedg.cpp	

local_C_INCLUDES+= 			\
	$(LOCAL_PATH)/ccmain		\
	$(LOCAL_PATH)/ccstruct 		\
	$(LOCAL_PATH)/cstruct 		\
	$(LOCAL_PATH)/cutil		\
	$(LOCAL_PATH)/ccutil 		\
	$(LOCAL_PATH)/ccmain 		\
	$(LOCAL_PATH)/image 		\
	$(LOCAL_PATH)/dict 		\
	$(LOCAL_PATH)/classify		\
	$(LOCAL_PATH)/pageseg		\
	$(LOCAL_PATH)/wordrec		\
	$(LOCAL_PATH)/textord

local_CFLAGS:=-DGRAPHICS_DISABLED
local_CFLAGS+=-DFST_DISABLED
local_CFLAGS+=-DDISABLE_DOC_DICT
local_CFLAGS+=-DDISABLE_INTEGER_MATCHING

#
# libocr (native)
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=$(local_SRC_FILES) ccmain/jni.cpp
LOCAL_C_INCLUDES:=$(local_C_INCLUDES)
LOCAL_CFLAGS:=$(local_CFLAGS)

LOCAL_SHARED_LIBRARIES:= \
	liblog

LOCAL_MODULE:= libocr

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)

ifeq ($(BUILD_FOR_HOST),1)

#
# libocr (host)
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=$(local_SRC_FILES)
LOCAL_C_INCLUDES:=$(local_C_INCLUDES)
LOCAL_CFLAGS:=$(local_CFLAGS)

LOCAL_MODULE:= libocr

include $(BUILD_HOST_SHARED_LIBRARY)

#
# tesseract test (host)
#

include $(CLEAR_VARS)

LOCAL_MODULE:= tesseract

LOCAL_SRC_FILES:= \
	ccmain/tesseractmain.cpp

LOCAL_CFLAGS:= \
	-DGRAPHICS_DISABLED	\
	-DFST_DISABLED

LOCAL_C_INCLUDES+= \
	$(LOCAL_PATH)/ccutil		\
	$(LOCAL_PATH)/ccstruct	\
	$(LOCAL_PATH)/image		\
	$(LOCAL_PATH)/dict		\
	$(LOCAL_PATH)/classify	\
	$(LOCAL_PATH)/wordrec	\
	$(LOCAL_PATH)/cutil		\
	$(LOCAL_PATH)/textord	\
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
	-DFST_DISABLED

LOCAL_C_INCLUDES+= \
	$(LOCAL_PATH)/ccutil		\
	$(LOCAL_PATH)/ccstruct	\
	$(LOCAL_PATH)/image		\
	$(LOCAL_PATH)/dict		\
	$(LOCAL_PATH)/classify	\
	$(LOCAL_PATH)/wordrec	\
	$(LOCAL_PATH)/cutil		\
	$(LOCAL_PATH)/textord	\
	$(LOCAL_PATH)/ccmain

LOCAL_SHARED_LIBRARIES:= \
	libocr

include $(BUILD_HOST_EXECUTABLE)

endif #BUILD_FOR_HOST

#
# libhelium (common)
#

local_SRC_FILES:= \
	helium/binarize.cpp \
	helium/cluster.cpp \
	helium/clusterer.cpp \
	helium/color.cpp \
	helium/cstringutils.cpp \
	helium/contourdetector.cpp \
	helium/debugging.cpp \
	helium/edgedetector.cpp \
	helium/gaussiansmooth.cpp \
	helium/heliumbinarizer.cpp \
	helium/histogram.cpp \
	helium/image.cpp \
	helium/mask.cpp \
	helium/maxtracer.cpp \
	helium/mathfunctions.cpp \
	helium/perspectivedetection.cpp \
	helium/point.cpp \
	helium/quicksmooth.cpp \
	helium/refcount.cpp \
	helium/shape.cpp \
	helium/shapetree.cpp \
	helium/sobeledgedetector.cpp \
	helium/laplaceedgedetector.cpp \
	helium/surface.cpp \
	helium/stringutils.cpp \
	helium/tesseract.cpp \
	helium/textareas.cpp \
	helium/textclassifier.cpp \
	helium/textvalidator.cpp \
	helium/textrecognition.cpp \
	helium/thresholdbinarizer.cpp \
	helium/trace.cpp \
	helium/tracer.cpp \
	helium/unionfind.cpp \
	helium/heliumtextdetector.cpp \
	helium/box.cpp \
	helium/imageenhancer.cpp 

local_C_INCLUDES:= \
	$(LOCAL_PATH)		\
	$(LOCAL_PATH)/ccmain         \
	$(LOCAL_PATH)/helium		\
	$(LOCAL_PATH)/ccutil		\
	$(LOCAL_PATH)/ccstruct	\
	$(LOCAL_PATH)/image		\
	$(LOCAL_PATH)/dict		\
	$(LOCAL_PATH)/classify	\
	$(LOCAL_PATH)/wordrec	\
	$(LOCAL_PATH)/cutil		\
	$(LOCAL_PATH)/textord	\

local_CFLAGS:= \
	-DGRAPHICS_DISABLED	\
	-DFST_DISABLED

#
# libhelium (native)
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=$(local_SRC_FILES)
LOCAL_CFLAGS:=$(local_CFLAGS)
LOCAL_C_INCLUDES:=$(local_C_INCLUDES)

LOCAL_MODULE:= libhelium

LOCAL_SHARED_LIBRARIES:= \
	libocr

LOCAL_PRELINK_MODULE:= false
include $(BUILD_SHARED_LIBRARY)
#include $(BUILD_STATIC_LIBRARY)

ifeq ($(BUILD_FOR_HOST),1)

#
# libhelium (host)
#

include $(CLEAR_VARS)

LOCAL_SRC_FILES:=$(local_SRC_FILES)
LOCAL_CFLAGS:=$(local_CFLAGS)
LOCAL_C_INCLUDES:=$(local_C_INCLUDES)

LOCAL_MODULE:= libhelium

LOCAL_SHARED_LIBRARIES:= \
	libocr

include $(BUILD_HOST_SHARED_LIBRARY)

#
# helium executable 
#

include $(CLEAR_VARS)

LOCAL_MODULE:= heliumtest

LOCAL_SRC_FILES:= \
	helium/test.cpp

LOCAL_CFLAGS:= 				\
	-DGRAPHICS_DISABLED		\
	-DFST_DISABLED

LOCAL_C_INCLUDES+= 			\
	$(LOCAL_PATH)			\
	$(LOCAL_PATH)/ccutil	\
	$(LOCAL_PATH)/ccstruct	\
	$(LOCAL_PATH)/image		\
	$(LOCAL_PATH)/dict		\
	$(LOCAL_PATH)/classify	\
	$(LOCAL_PATH)/wordrec	\
	$(LOCAL_PATH)/cutil		\
	$(LOCAL_PATH)/textord	\
	$(LOCAL_PATH)/ccmain

LOCAL_SHARED_LIBRARIES:= \
	libhelium \
	libocr

include $(BUILD_HOST_EXECUTABLE)
endif # BUILD_FOR_HOST
