#!/bin/bash


   SOURCE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

   function usage()
   {
      printf "\\tUsage: %s \\n\\t[Build Option -o <Debug|Release|RelWithDebInfo|MinSizeRel>] \\n\\t[CodeCoverage -c] \\n\\t[Doxygen -d] \\n\\t[CoreSymbolName -s <1-7 characters>] \\n\\t[Avoid Compiling -a]\\n\\n" "$0" 1>&2
      exit 1
   }

   ARCH=$( uname )
   if [ "${SOURCE_DIR}" == "${PWD}" ]; then
      BUILD_DIR="${PWD}/build"
   else
      BUILD_DIR="${PWD}"
   fi
   CMAKE_BUILD_TYPE=Release
   DISK_MIN=20
   DOXYGEN=false
   ENABLE_COVERAGE_TESTING=false
   CORE_SYMBOL_NAME="SYS"
   # Use current directory's tmp directory if noexec is enabled for /tmp
   if (mount | grep "/tmp " | grep --quiet noexec); then
        mkdir -p $SOURCE_DIR/tmp
        TEMP_DIR="${SOURCE_DIR}/tmp"
        rm -rf $SOURCE_DIR/tmp/*
   else # noexec wasn't found
        TEMP_DIR="/tmp"
   fi
   START_MAKE=true
   TIME_BEGIN=$( date -u +%s )
   VERSION=1.2

   txtbld=$(tput bold)
   bldred=${txtbld}$(tput setaf 1)
   txtrst=$(tput sgr0)

   if [ $# -ne 0 ]; then
      while getopts ":cdo:s:ah" opt; do
         case "${opt}" in
            o )
               options=( "Debug" "Release" "RelWithDebInfo" "MinSizeRel" )
               if [[ "${options[*]}" =~ "${OPTARG}" ]]; then
                  CMAKE_BUILD_TYPE="${OPTARG}"
               else
                  printf "\\n\\tInvalid argument: %s\\n" "${OPTARG}" 1>&2
                  usage
                  exit 1
               fi
            ;;
            c )
               ENABLE_COVERAGE_TESTING=true
            ;;
            d )
               DOXYGEN=true
            ;;
            s)
               if [ "${#OPTARG}" -gt 7 ] || [ -z "${#OPTARG}" ]; then
                  printf "\\n\\tInvalid argument: %s\\n" "${OPTARG}" 1>&2
                  usage
                  exit 1
               else
                  CORE_SYMBOL_NAME="${OPTARG}"
               fi
            ;;
            a)
               START_MAKE=false
            ;;
            h)
               usage
               exit 1
            ;;
            \? )
               printf "\\n\\tInvalid Option: %s\\n" "-${OPTARG}" 1>&2
               usage
               exit 1
            ;;
            : )
               printf "\\n\\tInvalid Option: %s requires an argument.\\n" "-${OPTARG}" 1>&2
               usage
               exit 1
            ;;
            * )
               usage
               exit 1
            ;;
         esac
      done
   fi

   if [ ! -d "${SOURCE_DIR}/.git" ]; then
      printf "\\n\\tThis build script only works with sources cloned from git\\n"
      printf "\\tPlease clone a new eos directory with 'git clone https://github.com/EOSIO/eos --recursive'\\n"
      printf "\\tSee the wiki for instructions: https://github.com/EOSIO/eos/wiki\\n"
      exit 1
   fi

   pushd "${SOURCE_DIR}" &> /dev/null

   STALE_SUBMODS=$(( $(git submodule status --recursive | grep -c "^[+\-]") ))
   if [ $STALE_SUBMODS -gt 0 ]; then
      printf "\\n\\tgit submodules are not up to date.\\n"
      printf "\\tPlease run the command 'git submodule update --init --recursive'.\\n"
      exit 1
   fi

   printf "\\n\\tBeginning build version: %s\\n" "${VERSION}"
   printf "\\t%s\\n" "$( date -u )"
   printf "\\tUser: %s\\n" "$( whoami )"
   printf "\\tgit head id: %s\\n" "$( cat .git/refs/heads/master )"
   printf "\\tCurrent branch: %s\\n" "$( git rev-parse --abbrev-ref HEAD )"
   printf "\\n\\tARCHITECTURE: %s\\n" "${ARCH}"

   popd &> /dev/null

   if [ "$ARCH" == "Linux" ]; then

      if [ ! -e /etc/os-release ]; then
         printf "\\n\\tEOSIO currently supports Amazon, Centos, Fedora, Mint & Ubuntu Linux only.\\n"
         printf "\\tPlease install on the latest version of one of these Linux distributions.\\n"
         printf "\\thttps://aws.amazon.com/amazon-linux-ami/\\n"
         printf "\\thttps://www.centos.org/\\n"
         printf "\\thttps://start.fedoraproject.org/\\n"
         printf "\\thttps://linuxmint.com/\\n"
         printf "\\thttps://www.ubuntu.com/\\n"
         printf "\\tExiting now.\\n"
         exit 1
      fi

      OS_NAME=$( cat /etc/os-release | grep ^NAME | cut -d'=' -f2 | sed 's/\"//gI' )

      case "$OS_NAME" in
         "CentOS Linux")
            FILE="${SOURCE_DIR}/scripts/build_centos.sh"
            CXX_COMPILER=g++
            C_COMPILER=gcc
            export CMAKE=${HOME}/opt/cmake/bin/cmake
         ;;
         "Ubuntu")
            FILE="${SOURCE_DIR}/scripts/build_ubuntu.sh"
            CXX_COMPILER=clang++-4.0
            C_COMPILER=clang-4.0
         ;;
         *)
            printf "\\n\\tUnsupported Linux Distribution. Exiting now.\\n\\n"
            exit 1
      esac

      export BOOST_ROOT="${HOME}/opt/boost"
      OPENSSL_ROOT_DIR=/usr/include/openssl
   fi

   #${SOURCE_DIR}/scripts/clean_old_install.sh
   if [ $? -ne 0 ]; then
      printf "\\n\\tError occurred while trying to remove old installation!\\n\\n"
      exit -1
   fi

   . "$FILE"

   printf "\\n\\n>>>>>>>> ALL dependencies sucessfully found or installed . Installing EOSIO\\n\\n"
   printf ">>>>>>>> CMAKE_BUILD_TYPE=%s\\n" "${CMAKE_BUILD_TYPE}"
   printf ">>>>>>>> ENABLE_COVERAGE_TESTING=%s\\n" "${ENABLE_COVERAGE_TESTING}"
   printf ">>>>>>>> DOXYGEN=%s\\n\\n" "${DOXYGEN}"

   if [ ! -d "${BUILD_DIR}" ]; then
      if ! mkdir -p "${BUILD_DIR}"
      then
         printf "Unable to create build directory %s.\\n Exiting now.\\n" "${BUILD_DIR}"
         exit 1;
      fi
   fi

   if ! cd "${BUILD_DIR}"
   then
      printf "Unable to enter build directory %s.\\n Exiting now.\\n" "${BUILD_DIR}"
      exit 1;
   fi

   if [ -z "$CMAKE" ]; then
      CMAKE=$( command -v cmake )
   fi

   if ! "${CMAKE}" -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}" -DCMAKE_CXX_COMPILER="${CXX_COMPILER}" \
      -DCMAKE_C_COMPILER="${C_COMPILER}"  -DCORE_SYMBOL_NAME="${CORE_SYMBOL_NAME}" \
      -DOPENSSL_ROOT_DIR="${OPENSSL_ROOT_DIR}" -DBUILD_MONGO_DB_PLUGIN=false \
       ${LOCAL_CMAKE_FLAGS} "${SOURCE_DIR}"
   then
      printf "\\n\\t>>>>>>>>>>>>>>>>>>>> CMAKE building EOSIO has exited with the above error.\\n\\n"
      exit -1
   fi

   if [ "${START_MAKE}" == "false" ]; then
      printf "\\n\\t>>>>>>>>>>>>>>>>>>>> EOSIO has been successfully configured but not yet built.\\n\\n"
      exit 0
   fi

   if [ -z ${JOBS} ]; then JOBS=$CPU_CORE; fi # Future proofing: Ensure $JOBS is set (usually set in scripts/eosio_build_*.sh scripts)
   if ! make -j"${JOBS}"
   then
      printf "\\n\\t>>>>>>>>>>>>>>>>>>>> MAKE building EOSIO has exited with the above error.\\n\\n"
      exit -1
   fi

   TIME_END=$(( $(date -u +%s) - ${TIME_BEGIN} ))


   printf "\\n\\tProgram has been successfully built. %02d:%02d:%02d\\n\\n" $(($TIME_END/3600)) $(($TIME_END%3600/60)) $(($TIME_END%60))
