from flask import Flask, render_template
from flask import Response
from flask import request
from flask_cors import CORS, cross_origin
import json
import logging
import os, sys
app = Flask(__name__)
CORS(app)

global gl_experiment_name
filename = ""
count = 0

@app.route('/', methods = ['POST', 'GET'])
def render():
    print request.form
    data = request.form
    px = data["px"]
    py = data["py"]
    finger = data["finger"]
    isRecordLast = data['isRecordLast']
    # if key == "":
    # 	return 'EMPTY'
    global filename
    global count

    if isRecordLast == "false":
    	#Naive one
    	filename = "./" + gl_experiment_name + "_naive.txt"
    	file = open(filename, 'w+')
    	print ("saving it in this file ", filename)
    	file.write(px + ' ' + py + ' ' + finger + '\n')
    	file.close()
    else:
    	#Naive one
    	filename = "./" + gl_experiment_name + "_naive.txt"
    	file = open(filename, 'a+')
    	file.write(px + ' ' + py + ' ' + finger + '\n')
    	file.close()
	    	
    return 'OK'

if __name__=='__main__':
	print ("this is the main function")
	global gl_experiment_name
	gl_experiment_name = sys.argv[1]
 	app.run(debug=True)
