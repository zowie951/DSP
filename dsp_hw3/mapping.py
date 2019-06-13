#coding=utf-8
import os
import sys


argv = sys.argv

if(len(argv)<3):
    print ("Usage: python mapping.py input_file output_file")
    exit()
input_file = argv[1]
output_file = argv[2]

f = open(input_file,'r',encoding='cp950')

dic = {}
for line in f.readlines():
   val,key = line.split(' ')
   key = key.split('/')
   key[-1] = key[-1][:-1]
   for i in key:
        if i[0] in dic:
            dic[i[0]].add(val)
        else:
            dic[i[0]] = set(val)

f.close()
f = open(output_file,'w',encoding='cp950')

uniq = set()
for i in dic:
    val = ' '.join(dic[i])
    s = "{} {}\n".format(i,val)
    f.write(s)
    for j in dic[i]:
        if j in uniq:
            continue
        f.write(str(j)+' '+str(j)+'\n')
        uniq.add(j)


