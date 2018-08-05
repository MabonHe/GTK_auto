#!/bin/bash

res_file_summary="test_results_summary"
res_file_all="test_results_all"
res_file_pass="test_result_pass"
res_file_fail="test_results_fail"

log_file=($res_file_summary $res_file_all $res_file_pass $res_file_fail)

#Remove existing logs file
for ((i=0; i<${#log_file[@]}; i++)); do
    if [ -f "${log_file[i]}" ]; then
        rm "${log_file[i]}"
    fi
done

#Calculate total/pass/fail cases number
for casename in `ls -l | grep ^d | awk '{print $NF}'`
do
    cat $casename"/"$casename".std" | grep "Result: PASS" > /dev/null
    if [ $? == 0 ]; then
            result="PASS"
            echo "$casename" >> $res_file_pass
    else
            result="FAIL"
            echo "$casename" >> $res_file_fail
    fi
    echo "$casename|$result" >> $res_file_all
done

#echo the total/pass/fail cases numbers
total_num=`grep "$line" $res_file_all | wc -l`
pass_num=`grep "$line" $res_file_all | grep -c "PASS"`
fail_num=`grep "$line" $res_file_all | grep -c "FAIL"`
echo "Total cases numbers: $total_num" >> $res_file_summary
echo "Pass numbers: $pass_num" >> $res_file_summary
echo "Fail numbers: $fail_num" >> $res_file_summary
cat $res_file_summary
echo "All cases list is in file $res_file_all"
echo "Pass cases list is in file $res_file_pass"
echo "Fail cases list is in file $res_file_fail"
echo "Test result summary is in file $res_file_summary"
