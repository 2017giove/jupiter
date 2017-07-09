########################################################
#
#  Makefile for drsosc, drscl and drs_exam 
#  executables under linux
#
#  Requires wxWidgets 2.8.9 or newer
#
#  161211:  modified by S. Veneziano to compile drs_sub
#  042017:  modified by J. UpiTer to compile PowerExpert
#  052017:  modified by J. UpiTer to compiler drs_expert
#
########################################################

# check if wxWidgets is installed
HAVE_WX       = $(shell which wx-config)
ifeq ($(HAVE_WX),)
$(error Error: wxWidgets required to compile "drsosc")
endif

# check for OS
OS            = $(shell uname)
ifeq ($(OS),Darwin)
DOS           = OS_DARWIN
else
DOS           = OS_LINUX
endif

CFLAGS        = -g -O2 -Wall -Wuninitialized -fno-strict-aliasing -std=gnu++0x -Iinclude -I/usr/local/include  -D$(DOS) -DHAVE_USB -DHAVE_LIBUSB10 -DUSE_DRS_MUTEX #`root-config --cflags`
LIBS          = -lpthread -lutil -lusb-1.0  #`root-config --glibs`


ifeq ($(OS),Darwin)
#CFLAGS        += -stdlib=libstdc++
#CFLAGS        += -stdlib=libc++ -std=c++11
CFLAGS        += -stdlib=libc++ 
endif         

# wxWidgets libs and flags
WXLIBS        = $(shell wx-config --libs)
WXFLAGS       = $(shell wx-config --cxxflags)

CPP_OBJ       = DRS.o averager.o ConfigDialog.o DOFrame.o DOScreen.o DRSOsc.o MeasureDialog.o Measurement.o Osci.o InfoDialog.o DisplayDialog.o AboutDialog.o EPThread.o TriggerDialog.o rb.o 
OBJECTS       = musbstd.o mxml.o strlcpy.o 


ifeq ($(OS),Darwin)
all: drsosc drscl DRSOsc.app PowerExpert drs_expert ThermoParanoid
else
all: drsosc drscl PowerExpert drs_expert ThermoParanoid
endif


CCFLAGS=	-DUNIX -DLINUX 
CLIBS=		-lcaenhvwrapper -lncurses -lpthread -ldl -lm
CINCLUDEDIR=	-I./$(GLOBALDIR) -I./include/
INCLUDES=	include/HVPowerSupply.h include/CAENHVWrapper.h 


HVPowerSupply.o: src/HVPowerSupply.cpp include/HVPowerSupply.h 
	g++ $(CFLAGS) $(CINCLUDEDIR) -c $<
	
PowerExpert: HVPowerSupply.o PowerExpert.o HVPowerSupply.o
	g++ $(FLAGS)  $(CINCLUDEDIR)  PowerExpert.o HVPowerSupply.o -o PowerExpert $(CLIBS) 


PowerExpert.o: src/PowerExpert.cpp src/HVPowerSupply.cpp  include/HVPowerSupply.h  
	g++ $(CFLAGS) $(CINCLUDEDIR) -c $<
	
ThermoParanoid: ThermoParanoid.o 
	g++ $(FLAGS)  $(CINCLUDEDIR)  ThermoParanoid.o -o ThermoParanoid $(CLIBS) 

ThermoParanoid.o: src/ThermoParanoid.cpp src/SerialClass.cpp  include/SerialClass.h  
	g++ $(CFLAGS) $(CINCLUDEDIR) -c $<

DRSOsc.app: drsosc
	-mkdir DRSOsc.app
	-mkdir DRSOsc.app/Contents
	-mkdir DRSOsc.app/Contents/MacOS
	-mkdir DRSOsc.app/Contents/Resources
	-mkdir DRSOsc.app/Contents/Resources/English.lproj
	echo 'APPL????' > DRSOsc.app/Contents/PkgInfo
	cp Info.plist DRSOsc.app/Contents/Info.plist
	cp DRSOsc.icns DRSOsc.app/Contents/Resources
	cp drsosc DRSOsc.app/Contents/MacOS/DRSOsc

drsosc: $(OBJECTS) $(CPP_OBJ) main.o
	$(CXX) $(CFLAGS) $(OBJECTS) $(CPP_OBJ) main.o -o drsosc $(LIBS) $(WXLIBS)

drscl: $(OBJECTS) DRS.o averager.o drscl.o
	$(CXX) $(CFLAGS) $(OBJECTS) DRS.o averager.o drscl.o -o drscl $(LIBS) $(WXLIBS)

drs_exam: $(OBJECTS) DRS.o averager.o drs_exam.o
	$(CXX) $(CFLAGS) $(OBJECTS) DRS.o averager.o drs_exam.o -o drs_exam $(LIBS) $(WXLIBS)
	

drs_sub: $(OBJECTS) DRS.o averager.o drs_sub.o
	$(CXX) $(CFLAGS) `root-config --cflags` $(OBJECTS) DRS.o averager.o drs_sub.o -o drs_sub $(LIBS) `root-config --libs` $(WXLIBS)

drs_jupiter: $(OBJECTS) DRS.o averager.o drs_jupiter.o
	$(CXX) $(CFLAGS) `root-config --cflags` $(OBJECTS) DRS.o averager.o drs_jupiter.o -o drs_jupiter $(LIBS) `root-config --libs` $(WXLIBS)

drs_expert: $(OBJECTS) HVPowerSupply.o DRS.o averager.o drs_expert.o HVPowerSupply.o
	$(CXX) $(CFLAGS) `root-config --cflags` $(OBJECTS) DRS.o averager.o HVPowerSupply.o drs_expert.o -o drs_expert $(LIBS) $(CLIBS)  `root-config --libs` -lncurses $(WXLIBS)

drs_exam_multi: $(OBJECTS) DRS.o averager.o drs_exam_multi.o
	$(CXX) $(CFLAGS) $(OBJECTS) DRS.o averager.o drs_exam_multi.o -o drs_exam_multi $(LIBS) $(WXLIBS)
	
main.o: src/main.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) $(WXFLAGS) -c $<

drscl.o: src/drscl.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c $<

drs_exam.o: src/drs_exam.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c $<

drs_sub.o: src/drs_sub.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) `root-config --cflags` -c $<

drs_jupiter.o: src/drs_jupiter.cpp include/mxml.h include/DRS.h 
	$(CXX) $(CFLAGS) `root-config --cflags` -c $<

drs_expert.o: src/drs_expert.cpp src/Waveform.cpp src/defines.h src/ChargeHist.cpp src/Cs_fit.cpp src/drs_menu.cpp src/volt_fit.cpp include/mxml.h include/DRS.h src/HVPowerSupply.cpp  include/HVPowerSupply.h   
	$(CXX) $(CFLAGS)  `root-config --cflags` -c $<

drs_exam_multi.o: src/drs_exam_multi.cpp include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) -c $<

$(CPP_OBJ): %.o: src/%.cpp include/%.h include/mxml.h include/DRS.h
	$(CXX) $(CFLAGS) $(WXFLAGS) -c $<

$(OBJECTS): %.o: src/%.c include/mxml.h include/DRS.h
	$(CC) $(CFLAGS) -c $<
	
clean:
	rm -f *.o 

