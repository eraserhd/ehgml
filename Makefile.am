SUBDIRS				= docs examples

bin_PROGRAMS			= ehgml

BUILT_SOURCES			= gml-lex.cc

ehgml_SOURCES			= ehgml.cc\
				  gml-lex.cc\
				  CGml.cc\
				  CArray.cc\
				  CObject.cc\
				  CObjectHeap.cc\
				  CClosure.cc\
				  CScalar.cc\
				  op.cc\
				  CLight.cc\
				  CModel.cc\
				  CRenderer.cc\
				  CMatrix.cc\
				  CPoint.cc\
				  CHitInfo.cc\
				  CGmlError.cc

noinst_HEADERS			= CGml.hh\
				  CHitInfo.hh\
				  CLight.hh\
				  CClosure.hh\
				  CMatrix.hh\
				  CModel.hh\
				  CPoint.hh\
				  CRay.hh\
				  CRenderer.hh\
				  CScalar.hh\
				  CObjectHeap.hh\
				  op.hh\
				  TObjectRef.hh\
				  CObject.hh\
				  CArray.hh\
				  CGmlError.hh\
				  macros.hh

EXTRA_DIST			= gml.lex

clean-local:
	rm -f *~

gml-lex.cc: gml.lex
	$(LEX) -ogml-lex.cc gml.lex
