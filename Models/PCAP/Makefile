# The Makefile for PCAP
#
# @author Nick
# @date 2024/09/24

# ######## [Necessary Common MakeFile Variables Included]
include ${CommonTempMakefile}

# If someone feels like using the definitions that users define, please use target-specific for reassign variables as below:
# CC			=
# STD			=
# CMPOPT		=
# DETAILINFO	=
# WALL		=
# FMSG		=
# CFLAGS		=
# LDFLAGS		=
# LDLIBS		=


# ######## [Environment Setting]
# Necessary MakeFile Variables Included
include ${TempMakefile}


# ######## [Variable Definition]
Models			:=	Models.PCAP
Duration		:=
PCAP.Headers	:=	${projectDir}/Models/PCAP/Headers
PCAP.Sources	:=	${projectDir}/Models/PCAP/Sources


# ######## [Passing the Variable Definition into the File]
# Verifying the content the file, ${TempMakefile} (tmp.mk from the project);
# the regular expression is e.g., grep -q "^PCAP\.Headers\s*\(:\)\?=" /home/dbsecure/native-sizing/tmp.mk && echo $?
# when the returned value is 0, the string contains the pattern; when the returned values are other values except 0, 
# the string does not contain the pattern
IsExisted	:=


# ######## [Implicit Rules]
# The definition for the basic element in the Makefile
.Suffixs: .c .h .cpp .hpp


# ######## [Phonies]
# The top level of the Makefile
.Phony: all
all: \
	startTime \
	${PCAP.Sources}/PCAPPrototype.o \
	${PCAP.Sources}/LinuxPCAP.o \
	endTime
# Adding the Makefile's variables shall be added into the ${TempMakefile} file
	@make information


# This section is for determining if the Makefile's variables shall be added into the 
# ${TempMakefile} file
.Phony: information
information:
# Verifying the content the file, ${TempMakefile} (tmp.mk from the project) to
# determine if the variable in the Makefile shall be added into the ${TempMakefile}
# Using the "eval" to assign the Makefile's variable in the recipe
	@$(eval IsExisted	:=	$(shell grep -q "^PCAP\.Sources\s*\(:\)\?=" "${TempMakefile}"  && echo "$$?"))
# Adding the variable into the file when the variables do not in the file, the information shall put into the file
	@if [ "${IsExisted}" != "0" ]; then \
		echo "PCAP.Sources	:=	${projectDir}/Models/PCAP/Sources" >> "${TempMakefile}"; \
	fi


# ######## [Compiling Items]
# Start time
startTime:
	@$(eval Duration := $(shell source ${projectDir}/Shells/compiledStringMessage.sh && echo $$(startingString "${Models}")))
# End Time
endTime:
	@echo $(shell source ${projectDir}/Shells/compiledStringMessage.sh && echo $$(endingString "${Models}" "${Duration}"))

# PCAP.PCAPPrototype
${PCAP.Sources}/PCAPPrototype.o:	${PCAP.Sources}/PCAPPrototype.cpp
	${CC} ${STD} ${CMPOPT} ${DETAILINFO} ${WALL} ${CFLAGS} ${PCAP.Sources}/PCAPPrototype.cpp -c ${FMSG} -o ${PCAP.Sources}/PCAPPrototype.o

# PCAP.LinuxPCAP
${PCAP.Sources}/LinuxPCAP.o:	${PCAP.Sources}/LinuxPCAP.cpp
	${CC} ${STD} ${CMPOPT} ${DETAILINFO} ${WALL} ${CFLAGS} ${PCAP.Sources}/LinuxPCAP.cpp -c ${FMSG} -o ${PCAP.Sources}/LinuxPCAP.o