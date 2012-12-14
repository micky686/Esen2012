import re
import sys


LABEL = r"[A-Za-z][A-Za-z0-9]{0,5}"
MNEMONIC = r"add|sub|div|mul|jmpgr|jmpeq|jmpls|compare|move|clone|die|sendmsg|pullmsg|storecr|store|mv|wait|priority|setservice|getservice"
REG = r"acc|reg_0|reg_1|reg_2|reg_3|reg_4|reg_5|reg_6|reg_str_0|reg_str_1|reg_str_2"
VALUE = r"0x[0-9A-F]{1,2}|0b[01]{1,8}|[0-9]{1,3}"
SERVICE = r"temp|bargraph|fan|heater|led_matrix"
CHAR = r"[A-Za-z0-9_]"
OPERAND = REG + "|" + VALUE + "|" + SERVICE + "|" + CHAR
LEGAL_LABEL = "((?!" + MNEMONIC + "|" + OPERAND + ")" + LABEL +")"
LABEL_LOOKUP = "(?:" + LEGAL_LABEL + "\s*:)?" + "\s*(" + MNEMONIC + ")" + "(?:\s+" + LEGAL_LABEL + ")?"
OPERAND_LOOKUP = "(?:" + LEGAL_LABEL + "\s*:)?" + "\s*(" + MNEMONIC + ")" + "(?:\s+(" + OPERAND + "))?\s*(?:,\s*(" + OPERAND + "))?\s*(?:,\s*(" + OPERAND + "))?\s*"

reg = {"reg_0":"0000",\
           "reg_1":"0001",\
           "reg_2":"0010",\
           "reg_3":"0011",\
           "reg_4":"0100",\
           "reg_5":"0101",\
           "reg_6":"0110",\
           "acc"  :"1111"}

reg_str = {"reg_str_0":"1000",\
              "reg_str_1":"1001",\
              "reg_str_2":"1010"}

service = {"temp":"0",\
               "bargraph":"1",\
               "fan":"2",\
               "heater":"3",\
               "led_matrix":"4",\
               }

command_opcode = {"add":"00000011",\
               "addv":"0011",\
               "sub":"00000110",\
               "subv":"0110",\
               "div":"00001001",\
               "divv":"1001",\
               "mul":"00001100",\
               "mulv":"1100",\
               "jmpgr":"11110011",\
               "jmpeq":"11110110",\
               "jmpls":"11111100",\
               "compare":"00001010",\
               "comparev":"1010",\
               "move":"11110001",\
               "clone":"1111001000000000",\
               "die":"1111010000000000",\
               "sendmsg":"11111000",\
               "pullmsg":"111110100000",\
               "store":"1101",\
               "storecr":"1011",\
               "mv":"00001101",\
               "wait":"00000101",\
               "priority":"00001000",\
               "setservice":"0111",\
               "getservice":"00000111"}
               
#LEGAL LINE:
#LABEL:? MNEMONIC (?: OPERAND1? (?:, OPERAND2?)) | (LABEL2)
FILENAME = r""
SOURCE_FILE = r""
FIRST_PARS = {}

def asm_tool(filename):
    if check_files(filename) == None:
        return None
    first_pass()
    second_pass()


def first_pass():
    source = open(SOURCE_FILE , "r")
    num_lines = sum(1 for line in open(SOURCE_FILE))
    i,k=1,0
    while i <= num_lines:
        source_line = source.readline()
        label_lookup = re.compile(LABEL_LOOKUP).match(source_line)
        operand_lookup = re.compile(OPERAND_LOOKUP).match(source_line)
        if label_lookup != None:
            label_1, label_2 = label_lookup.group(1), label_lookup.group(3)
            mnemonic = label_lookup.group(2)
            operand_1, operand_2, operand_3 = operand_lookup.group(3), operand_lookup.group(4), operand_lookup.group(5)
            FIRST_PARS[k] = {"LABEL1":label_1,"MNEMONIC":mnemonic,"LABEL2":label_2,\
                                 "OPERAND1":operand_1, "OPERAND2":operand_2, "OPERAND3":operand_3, "LINE":i}
            k+=1
        i+=1
    source.close()
    
def second_pass():
#    print FIRST_PARS
    binary = open(FILENAME + ".bin", "w+")
    listing = open(FILENAME + ".lst", "w+")
    i = 0

    while i < len(FIRST_PARS):
        #assemble line
        opcode = ""
        mnem = FIRST_PARS[i]["MNEMONIC"].lower()
        # 2 operand reg-reg or reg-value commands
        if  mnem == "add" or mnem == "sub" or mnem == "mul" or mnem == "div" or mnem == "compare":
           if FIRST_PARS[i]["LABEL2"] != None:
               print "Error in line {}, illegal label ".format(FIRST_PARS[i]["LINE"])
           elif  FIRST_PARS[i]["OPERAND1"] not in reg or FIRST_PARS[i]["OPERAND2"] == None or FIRST_PARS[i]["OPERAND3"] != None:
               print "Error in line {}, illegal operand".format(FIRST_PARS[i]["LINE"])
           elif FIRST_PARS[i]["OPERAND2"] in reg:
               opcode = command_opcode[mnem] + reg[FIRST_PARS[i]["OPERAND1"]]+reg[FIRST_PARS[i]["OPERAND2"]]
           elif re.compile(VALUE).match(FIRST_PARS[i]["OPERAND2"]) != None:
               opcode = command_opcode[mnem + "v"] + reg[FIRST_PARS[i]["OPERAND1"]] + convert_to_binary(FIRST_PARS[i]["OPERAND2"])
        #0 operand 1 label commands
        if mnem == "jmpeq" or mnem == "jmpls" or mnem == "jmpgr":
            if FIRST_PARS[i]["OPERAND1"] != None or FIRST_PARS[i]["OPERAND2"] != None or FIRST_PARS[i]["OPERAND3"] != None:
                print "Error in line {}, illegal operand".format(FIRST_PARS[i]["LINE"])
            elif FIRST_PARS[i]["LABEL2"] == None:
                print "Error in line {}, illegal label".format(FIRST_PARS[i]["LINE"])
            else:
            #TODO
                offset = 1#rewrite
               # offset = convert_to_binary(offset)
                opcode = command_opcode[mnem] + offset
        #0 operand commands
        if mnem == "clone" or mnem == "die":
            if FIRST_PARS[i]["OPERAND1"] != None or FIRST_PARS[i]["OPERAND2"] != None or FIRST_PARS[i]["OPERAND3"] != None:
                print "Error in line {}, illegal operand".format(FIRST_PARS[i]["LINE"])
            elif FIRST_PARS[i]["LABEL2"] != None:
                print "Error in line {}, illegal label ".format(FIRST_PARS[i]["LINE"])
            else:
                opcode = command_opcode[mnem]
        #1 operand commands
        if mnem == "move" or mnem == "pullmsg" or mnem == "wait" or mnem == "priority" or mnem == "getservice":
            if FIRST_PARS[i]["OPERAND1"] == None or FIRST_PARS[i]["OPERAND2"] != None or FIRST_PARS[i]["OPERAND3"] != None:
                print "Error in line {}, illegal operand".format(FIRST_PARS[i]["LINE"])
            elif FIRST_PARS[i]["LABEL2"] != None:
                print "Error in line {}, illegal label ".format(FIRST_PARS[i]["LINE"])
            elif mnem == "move" or mnem == "getservice":
                if FIRST_PARS[i]["OPERAND1"] not in service:
                    print  "Error in line {}, illegal operand".format(FIRST_PARS[i]["LINE"])
                else:
                    opcode = command_opcode[mnem] + convert_to_binary(service[FIRST_PARS[i]["OPERAND1"]])
            elif mnem == "pullmsg":
                if FIRST_PARS[i]["OPERAND1"] in reg:
                    opcode = command_opcode[mnem] + reg[FIRST_PARS[i]["OPERAND1"]]
                elif FIRST_PARS[i]["OPERAND1"] in reg_str:
                    opcode = command_opcode[mnem] + reg_str[FIRST_PARS[i]["OPERAND1"]]
                else:
                    print  "Error in line {}, illegal message destination".format(FIRST_PARS[i]["LINE"])
            elif mnem == "wait":
                if re.compile(VALUE).match(FIRST_PARS[i]["OPERAND1"]) != None:
                    opcode = command_opcode[mnem] + convert_to_binary(FIRST_PARS[i]["OPERAND1"])
                else:
                    print  "Error in line {}, illegal delay value".format(FIRST_PARS[i]["LINE"])
            elif mnem == "priority":
                if re.compile("[0-3]").match(FIRST_PARS[i]["OPERAND1"]) != None:
                    opcode = command_opcode[mnem] + convert_to_binary(FIRST_PARS[i]["OPERAND1"])
                else:
                    print "Error in line {}, illegal priority value".format(FIRST_PARS[i]["LINE"])
        if mnem == "store" or mnem == "storecr":
            if FIRST_PARS[i]["LABEL2"] != None:
               print "Error in line {}, illegal label ".format(FIRST_PARS[i]["LINE"])
            elif mnem == "store" and FIRST_PARS[i]["OPERAND1"] in reg:
                opcode = command_opcode[mnem] + reg[FIRST_PARS[i]["OPERAND1"]] + convert_to_binary(FIRST_PARS[i]["OPERAND2"])
            elif mnem == "storecr" and FIRST_PARS[i]["OPERAND1"] in reg_str:
                opcode = command_opcode[mnem] + reg_str[FIRST_PARS[i]["OPERAND1"]] + convert_to_binary(str(ord(FIRST_PARS[i]["OPERAND2"])))
            else:
                print "Error in line {}, operands do not match mnemonic ".format(FIRST_PARS[i]["LINE"])
        if mnem == "setservice":
            if FIRST_PARS[i]["LABEL2"] != None:
                print "Error in line {}, illegal label ".format(FIRST_PARS[i]["LINE"])
            elif FIRST_PARS[i]["OPERAND1"] in service and FIRST_PARS[i]["OPERAND2"] in reg:
                opcode = command_opcode[mnem] + reg[FIRST_PARS[i]["OPERAND2"]] + convert_to_binary(service[FIRST_PARS[i]["OPERAND1"]])
            else:
                print  "Error in line {}, illegal operand".format(FIRST_PARS[i]["LINE"])
        if mnem == "sendmsg":
            if FIRST_PARS[i]["LABEL2"] != None:
                 print "Error in line {}, illegal label ".format(FIRST_PARS[i]["LINE"])
            elif FIRST_PARS[i]["OPERAND1"] in reg and re.compile("[0-3]").match(FIRST_PARS[i]["OPERAND2"]) != None and \
                    re.compile("[0-3]").match(FIRST_PARS[i]["OPERAND3"]) != None:
                agent =  bin(int(FIRST_PARS[i]["OPERAND2"]))[2:]
                dest_agent = agent if len(agent) == 2 else "0" + agent
                platform = bin(int(FIRST_PARS[i]["OPERAND3"]))[2:]
                dest_platform = platform if len(platform) == 2 else "0" + platform
                opcode = command_opcode[mnem] + reg[FIRST_PARS[i]["OPERAND1"]] + dest_agent + dest_platform
            else:
                print  "Error in line {}, illegal operand".format(FIRST_PARS[i]["LINE"])
        
        if mnem == "mv":
            if FIRST_PARS[i]["LABEL2"] != None:
               print "Error in line {}, illegal label ".format(FIRST_PARS[i]["LINE"])
            elif  FIRST_PARS[i]["OPERAND1"] not in reg or FIRST_PARS[i]["OPERAND2"] not in reg or FIRST_PARS[i]["OPERAND3"] != None:
               print "Error in line {}, illegal operand".format(FIRST_PARS[i]["LINE"])
            else:
                opcode = command_opcode[mnem] + reg[FIRST_PARS[i]["OPERAND1"]] + reg[FIRST_PARS[i]["OPERAND2"]]
        
       # print FIRST_PARS[i]
       # print opcode
        binary.write(opcode)
        listing.write(xstr(FIRST_PARS[i]["LABEL1"]) + "\t\t" + opcode + "\t\t" + xstr(FIRST_PARS[i]["MNEMONIC"]) + "\t\t" + \
                         xstr(FIRST_PARS[i]["OPERAND1"]) + "\t\t" + xstr(FIRST_PARS[i]["OPERAND2"]) + "\t\t" + \
                          xstr(FIRST_PARS[i]["OPERAND3"]) + "\t\t" + xstr(FIRST_PARS[i]["LABEL2"]) + "\n")
        i+=1

    binary.close()
    listing.close()
        
def convert_to_binary(value):
    bin_value = ""
    if re.compile("^0x([0-9a-fA-F]{1,2})$").match(value) != None:
          bin_value = bin(int(re.compile("^0x([0-9a-fA-F]{1,2})$").match(value).group(1), 16))[2:]
    elif re.compile("^0b([01]{1,8})$").match(value) != None:
          bin_value = re.compile("^0b([01]{1,8})$").match(value).group(1)
    elif re.compile("^[+-]?([01]?[0-9]?[0-9])$").match(value) != None:
          #negative numbers
          bin_value = bin(int(re.compile("^[+-]?([01]?[0-9]?[0-9])$").match(value).group(1)))[2:]
    else:
          print "invalid value"
          return None
    while len(bin_value) < 8:
          bin_value = "0" + bin_value
    return bin_value
       

def check_files(filename):
    """check agent file for .ma extension, extract filename for listing, hex, binary generation"""
    SOURCE_REGEX = re.compile("([A-Za-z0-9_]+)\.ma").match(filename)
    if SOURCE_REGEX == None:
        print "\tInvalid file!\n\tSpecify agent file with extension .ma\t!\n"
        return None
    global SOURCE_FILE, FILENAME
    SOURCE_FILE = SOURCE_REGEX.group(0)
    FILENAME = SOURCE_REGEX.group(1)
   # print SOURCE_FILE + " to be assembled\n"
    return SOURCE_FILE

def xstr(s):
    if s == None:
        return ""
    else:
        return s

asm_tool(sys.argv[1])

