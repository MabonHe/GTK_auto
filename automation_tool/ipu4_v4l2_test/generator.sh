#!/bin/bash

# env
SHELL=/bin/bash
SHELL_FILE=run_all_cases.sh
CASES_DIR=TestCases
EXE_NAME=ipu4_v4l2_test
RES_CALC_FILE=test_results_calc.sh

# parameters
IMX214_BE_CONFIG=./imx214_be_config.sh
OV13860_BE_CONFIG=./ov13860_be_config.sh
ADV7481_mipi_CONFIG=./adv7481_mipi_config.sh

RESULT_FILE_PREFIX=frame_
BUFFER_TYPE=V4L2_BUF_TYPE_VIDEO_CAPTURE

DEBUG=0
if [ $DEBUG -eq 1 ]
then
    MEMORY_TYPE=( Mmap )
    IS_BLOCK_MODE=( Block )
    PIXEL_FORMAT=( Raw10 )
    # progressive resolution
    P_RES=( 1920x1080 )
    # interlaced resolution
    I_RES=( 1920x1080 )
else
    # MEMORY_TYPE=( Mmap Userptr Dmabuf)
    MEMORY_TYPE=( Mmap Userptr )
    IS_BLOCK_MODE=( Block Noblock )
    PIXEL_FORMAT=( Raw8 Raw10 YUV422 )
    # progressive resolution
    P_RES=( 1920x1080 1280x720 720x576 640x480 )
    # interlaced resolution
    I_RES=( 1920x1080 720x576 720x480 )
fi

if [ -d $CASES_DIR ]
then
    rm -rf $CASES_DIR
fi
mkdir $CASES_DIR

cp $EXE_NAME $RES_CALC_FILE $CASES_DIR

cd $CASES_DIR

# return casename_perfix cmd_perfix
generate_cmd_perfix() {
    CASE_LISTS=`./$EXE_NAME --gtest_list_tests | grep CI_PRI_IPU4_IOCTL_ | cut -c 3-`
    for case in $CASE_LISTS
    do
        echo $case
    done
}

# return casename_suffix cmd_suffix
generate_cmd_suffix() {
    for res in "${P_RES[@]}"
    do
        for mode in "${IS_BLOCK_MODE[@]}"
        do
            for mem in "${MEMORY_TYPE[@]}"
            do
                for format in "${PIXEL_FORMAT[@]}"
                do
                    width=`echo $res | cut -d 'x' -f 1`
                    height=`echo $res | cut -d 'x' -f 2`
                    echo $mode"_"$mem"_"$format"_"$res
                done
            done
        done
    done
}

get_cmd() {
    # $1 - perfix casename
    # $2 - suffix casename
    # $3 - shell/case
    # return: cmd
    suffix=$2

    _mode=`echo $suffix | cut -d '_' -f 1`
    if [ $_mode == "Block" ]
    then
        mode="true"
    else
        mode="false"
    fi

    _mem=`echo $suffix | cut -d '_' -f 2`
    if [ $_mem == "Mmap" ]
    then
        mem="V4L2_MEMORY_MMAP"
    elif [ $_mem == "Userptr" ]
    then
        mem="V4L2_MEMORY_USERPTR"
    else
        mem='V4L2_MEMORY_DMABUF'
    fi

    _format=`echo $suffix | cut -d '_' -f 3`
    if [ $_format == "Raw10" ]
    then
        format="V4L2_PIX_FMT_SGRBG10"
        config=$OV13860_BE_CONFIG
        echo "dev_name=\$(media-ctl -e \"Intel IPU4 CSI2 BE capture\")"
    elif [ $_format == "Raw8" ]
    then
        format="V4L2_PIX_FMT_SGRBG8"
        config=$IMX214_BE_CONFIG
        echo "dev_name=\$(media-ctl -e \"Intel IPU4 CSI2 BE capture\")"
    else
        format="V4L2_PIX_FMT_UYVY"
        config=$ADV7481_mipi_CONFIG
        echo "dev_name=\$(media-ctl -e \"Intel IPU4 CSI-2 0 capture\")"
    fi

    _res=`echo $suffix | cut -d '_' -f 4`
    width=`echo $_res | cut -d 'x' -f 1`
    height=`echo $_res | cut -d 'x' -f 2`

    if [ $3 == "fast" ]
    then
        echo -en ";"
        echo "./"$EXE_NAME" --gtest_filter=\"MC_Stdioctl_Test.*\" -d="'$dev_name'" -i="$mode" -m="$mem" -p="$format" -w="$width" -h="$height" -c="$config
    else
        echo -en ";"
        echo $EXE_NAME" --gtest_filter=\"MC_Stdioctl_Test."$1"\" -d="'$dev_name'" -i="$mode" -m="$mem" -p="$format" -w="$width" -h="$height" -c="$config
    fi
}

generate_config_file() {
    # $1 casename_suffix
    # $2 casename
    casename_suffix=$1
    fmt=`echo $casename_suffix | cut -d '_' -f 3`
    res=`echo $casename_suffix | cut -d '_' -f 4`

    if [ $fmt == "Raw10" ]
    then
        echo "#!"$SHELL > "./"$casename/$OV13860_BE_CONFIG
        echo "format=SGRBG10" >> "./"$casename/$OV13860_BE_CONFIG
        echo "res="$res >> "./"$casename/$OV13860_BE_CONFIG
        cat "../"$OV13860_BE_CONFIG >> "./"$casename/$OV13860_BE_CONFIG
        chmod +x "./"$casename/$OV13860_BE_CONFIG
        # dev_name=$(media-ctl -e "Intel IPU4 CSI-2 0 capture")
    elif [ $fmt == "Raw8" ]
    then
        echo "#!"$SHELL > "./"$casename/$IMX214_BE_CONFIG
        echo "format=SGRBG8" >> "./"$casename/$IMX214_BE_CONFIG
        echo "res="$res >> "./"$casename/$IMX214_BE_CONFIG
        cat "../"$IMX214_BE_CONFIG >> "./"$casename/$IMX214_BE_CONFIG
        chmod +x "./"$casename/$IMX214_BE_CONFIG
    else
        echo "#!"$SHELL > "./"$casename/$ADV7481_mipi_CONFIG
        echo "format=UYVY" >> "./"$casename/$ADV7481_mipi_CONFIG
        echo "res="$res >> "./"$casename/$ADV7481_mipi_CONFIG
        cat "../"$ADV7481_mipi_CONFIG >> "./"$casename/$ADV7481_mipi_CONFIG
        chmod +x "./"$casename/$ADV7481_mipi_CONFIG
    fi
}

generate_cmd() {
    perfixs=`generate_cmd_perfix`
    suffixs=`generate_cmd_suffix`

    echo "#!"$SHELL > $SHELL_FILE
    echo "# MC_Stdioctl_Test cases" >> $SHELL_FILE
    chmod +x $SHELL_FILE

    for casename_suffix in ${suffixs[@]}
    do
        if [ $1 == "fast" ]
        then
            echo `get_cmd "" $casename_suffix "shell"` >> $SHELL_FILE
        else
            for casename_perfix in ${perfixs[@]}
            do
                if [ $1 == 'cases' ]
                then
                    # generate case shell file
                    casename=`echo $casename_perfix"_"$casename_suffix`
                    echo $casename
                    mkdir $casename
                    echo "#!"$SHELL > "./"$casename"/"$casename
                    echo "CASE_NAME=$casename" >> "./"$casename"/"$casename
                    echo 'RESULT="PASS"' >> "./"$casename"/"$casename
                    echo 'DESCRIPTION=""' >> "./"$casename"/"$casename
                    echo `get_cmd $casename_perfix $casename_suffix "case"` >> "./"$casename"/"$casename
                    echo 'retval=`echo $?`' >> "./"$casename"/"$casename
                    echo 'if [ $retval -eq 1  ]; then' >> "./"$casename"/"$casename
                    echo '    RESULT="FAIL"' >> "./"$casename"/"$casename
                    echo '    DESCRIPTION="Error, please refer to ${CASE_NAME}.log."' >> "./"$casename"/"$casename
                    echo 'fi' >> "./"$casename"/"$casename
                    echo 'echo "Test Case: $CASE_NAME" > ./${CASE_NAME}/${CASE_NAME}.std' >> "./"$casename"/"$casename
                    echo 'echo "Result: $RESULT" >> ./${CASE_NAME}/${CASE_NAME}.std' >> "./"$casename"/"$casename
                    echo 'echo "Description: $DESCRIPTION" >> ./${CASE_NAME}/${CASE_NAME}.std' >> "./"$casename"/"$casename

                    chmod +x "./"$casename"/"$casename

                    echo "./$casename/$casename > ./$casename/$casename.log" >> $SHELL_FILE
                elif [ $1 == 'shell' ]
                then
                    # generate shell file
                    casename=`echo $casename_perfix"_"$casename_suffix`
                    echo $casename
                    mkdir $casename
                    echo `get_cmd $casename_perfix $casename_suffix "case"` >> "./"$casename"/"$casename

                    chmod +x "./"$casename"/"$casename

                    echo "./$casename/$casename > ./$casename/$casename.log" >> $SHELL_FILE
                fi

                # generate config file
                generate_config_file $casename_suffix $casename
            done
        fi
    done
}

if [ $# -eq 0 ]
then
    generate_cmd fast
else
    generate_cmd $1
fi
