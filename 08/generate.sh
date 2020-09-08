echo -e "#include <stdio.h>\nint main(void){\n"

# all registers - union of first words and the words after 'if'
registers=$(cat <(cut -f1 -d' ' input) <(sed 's/.*if \([^ ]*\) .*/\1/' input) | sort | uniq)

# register variable initialization
echo "$registers" | sed 's/\(.*\)/int \1 = 0;/'
echo "int total_max = 0, tmp=0;"

# transform instructions
sed -e 's/\(.*\)if\(.*\)/if (\2){tmp=\1;total_max=tmp>total_max?tmp:total_max;}/' -e 's/inc/+=/' -e 's/dec/-=/' input

#echo "$registers" | sed 's/\(.*\)/printf("\1 = %d\\n", \1);/'

echo "int all[] = {"
echo "$registers" |  paste -s -d','
echo '};int max=0;for(int i=0;i<sizeof(all)/sizeof(int);i++)if(max<all[i])max=all[i];printf("Part 1 result: %d\n",max);printf("Part 2 result: %d\n",total_max);'
echo -e "return 0;\n}"
