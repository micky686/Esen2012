import re
import sys


LABEL = r"[A-Za-z][A-Za-z0-9]{0,5}"
MNEMONIC = r"add|sub|div|mul|jmpgr|jmpeq|jmpls|compare|move|clr|clone|die|sendmsg|pullmsg|storecr|ldh|ldl|mv|wait|priority|setservice|getservice"
REG = r"reg_0|reg_1|reg_2|reg_3|reg_4|reg_5|reg_6|reg_str_0|reg_str_1|reg_str_2"
VALUE = r"0x[0-9A-F]{1,2}|0b[01]{1,8}|[+-]?[01]?[0-9]?[0-9]"
SERVICE = r"temp|bargraph|fan|heater|led_matrix"
CHAR = r"[A-Za-z0-9_]"
OPERAND = REG + "|" + VALUE + "|" + SERVICE 
OPERAND_OR_CHAR = OPERAND + "|" + CHAR
LEGAL_LABEL = "((?!" + MNEMONIC + "|" + OPERAND + ")" + LABEL +")"
LABEL_LOOKUP = "(?:" + LEGAL_LABEL + "\s*:)?" + "\s*(" + MNEMONIC + ")" + "(?:\s+" + LEGAL_LABEL + ")?"
OPERAND_LOOKUP = "(?:" + LEGAL_LABEL + "\s*:)?" + "\s*(" + MNEMONIC + ")" + "(?:\s+(" + OPERAND_OR_CHAR  + "))?\s*(?:,\s*(" + OPERAND_OR_CHAR + "))?\s*(?:,\s*(" + OPERAND_OR_CHAR + "))?\s*"

reg = {"reg_0":"0000",\
           "reg_1":"0001",\
           "reg_2":"0010",\
           "reg_3":"0011",\
           "reg_4":"0100",\
           "reg_5":"0101",\
           "reg_6":"0110",\
           }

reg_str = {"reg_str_0":"1000",\
              "reg_str_1":"1001",\
              "reg_str_2":"1010"}

service = {"temp":"1",\
               "bargraph":"0",\
               "fan":"2",\
               "heater":"3",\
               "led_matrix":"4",\
               "lcd":"5",\
               "buttons":"6",\
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
               "ldh":"1101",\
               "ldl":"0100", \
               "storecr":"1011",\
               "clr":"000000100000",\
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

def asm_tool():
    try:
        check_files(sys.argv[1])
        first_pass()
        second_pass()
    except (IOError, IndexError):
        print "\tSpecify correct filename"
    


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
    binary = open(FILENAME + ".bin", "w+")
    listing = open(FILENAME + ".lst", "w+")
    i = 0

    while i < len(FIRST_PARS):
        #assemble line
        opcode = ""
        mnem = FIRST_PARS[i]["MNEMONIC"].lower()
        label1 = None if FIRST_PARS[i]["LABEL1"] == None else FIRST_PARS[i]["LABEL1"]
        label2 =  None if FIRST_PARS[i]["LABEL2"] == None else FIRST_PARS[i]["LABEL2"]
        operand1 = None if FIRST_PARS[i]["OPERAND1"] == None else FIRST_PARS[i]["OPERAND1"].lower()
        operand2 =  None if FIRST_PARS[i]["OPERAND2"] == None else FIRST_PARS[i]["OPERAND2"].lower()
        operand3 =  None if FIRST_PARS[i]["OPERAND3"] == None else FIRST_PARS[i]["OPERAND3"].lower()
        line =  None if FIRST_PARS[i]["LINE"] == None else FIRST_PARS[i]["LINE"]
        
        # 2 operand reg-reg or reg-value commands
        if  mnem == "add" or mnem == "sub" or mnem == "mul" or mnem == "div" or mnem == "compare":
           if label2 != None:
               print "Error in line %s, illegal label " %line
           elif  operand1 not in reg or operand2 == None or operand3 != None:
               print "Error in line %s, illegal operand" %line
           elif operand2 in reg:
               opcode = command_opcode[mnem] + reg[operand1]+reg[operand2]
           elif re.compile(VALUE).match(operand2) != None:
               opcode = command_opcode[mnem + "v"] + reg[operand1] + convert_to_binary(operand2)
        #0 operand 1 label commands
        if mnem == "jmpeq" or mnem == "jmpls" or mnem == "jmpgr":
            label = []
            for lines in FIRST_PARS.iterkeys():
                   if FIRST_PARS[lines]["LABEL1"] == label2:
                       label.append(lines)
            if len(label) != 1 or label2 == None:
                print " Unresolved labels in line %s" %line
            else:
                offset = label[0] - i
                opcode = command_opcode[mnem] + convert_to_binary(str(offset))

        #0 operand commands
        if mnem == "clone" or mnem == "die":
            if operand1 != None or operand2 != None or operand3 != None:
                print "Error in line %s, illegal operand" %line
            elif label2 != None:
                print "Error in line %s, illegal label " %line
            else:
                opcode = command_opcode[mnem]
        #1 operand commands
        if mnem == "move" or mnem == "pullmsg" or mnem == "wait" or mnem == "priority" or mnem == "getservice" or mnem == "clr":
            if operand1 == None or operand2 != None or operand3 != None:
                print "Error in line %s, illegal operand" %line
            elif label2 != None:
                print "Error in line %s, illegal label " %line
            elif mnem == "move" or mnem == "getservice":
                if operand1 not in service:
                    print  "Error in line %s, illegal operand" %line
                else:
                    opcode = command_opcode[mnem] + convert_to_binary(service[operand1])
            elif mnem == "pullmsg":
                if operand1 in reg:
                    opcode = command_opcode[mnem] + reg[operand1]
                elif operand1 in reg_str:
                    opcode = command_opcode[mnem] + reg_str[operand1]
                else:
                    print  "Error in line %s, illegal message destination" %line
            elif mnem == "wait":
                if re.compile(VALUE).match(operand1) != None:
                    opcode = command_opcode[mnem] + convert_to_binary(operand1)
                else:
                    print  "Error in line %s, illegal delay value" %line
            elif mnem == "priority":
                if re.compile("[0-3]").match(operand1) != None:
                    opcode = command_opcode[mnem] + convert_to_binary(operand1)
                else:
                    print "Error in line %s, illegal priority value" %line
            elif mnem == "clr":
                if operand1 not in reg_str:
                    print "Error in line %s, illegal operand" %line
                else:
                    opcode = command_opcode[mnem] + reg_str[operand1]

        if mnem == "ldl" or mnem == "ldh" or mnem == "storecr":
            if label2 != None:
               print "Error in line %s, illegal label " %line
            elif mnem == "ldl" or mnem == "ldh" and operand1 in reg:
                opcode = command_opcode[mnem] + reg[operand1] + convert_to_binary(operand2)
            elif mnem == "storecr" and operand1 in reg_str:
                opcode = command_opcode[mnem] + reg_str[operand1] + convert_to_binary(str(ord(operand2)))
            else:
                print "Error in line %s, operands do not match mnemonic " %line
        if mnem == "setservice":
            if label2 != None:
                print "Error in line %s, illegal label " %line
            elif operand1 in service and operand2 in reg:
                opcode = command_opcode[mnem] + reg[operand2] + convert_to_binary(service[operand1])
            else:
                print  "Error in line %s, illegal operand" %line
        if mnem == "sendmsg":
            if label2 != None:
                 print "Error in line %s, illegal label " %line
            elif operand1 in reg and re.compile("[0-3]").match(operand2) != None and \
                    re.compile("[0-3]").match(operand3) != None:
                agent =  bin(int(operand2))[2:]
                dest_agent = agent if len(agent) == 2 else "0" + agent
                platform = bin(int(operand3))[2:]
                dest_platform = platform if len(platform) == 2 else "0" + platform
                opcode = command_opcode[mnem] + reg[operand1] + dest_agent + dest_platform
            else:
                print  "Error in line %s, illegal operand" %line
        
        if mnem == "mv":
            if label2 != None:
               print "Error in line %s, illegal label " %line
            elif  operand1 not in reg or operand2 not in reg or operand3 != None:
               print "Error in line %s, illegal operand" %line
            else:
                opcode = command_opcode[mnem] + reg[operand1] + reg[operand2]
        
        binary.write(opcode)
        listing.write(xstr(FIRST_PARS[i]["LABEL1"]) + "\t\t" + opcode + "\t\t" + xstr(FIRST_PARS[i]["MNEMONIC"]) + "\t\t" + \
                         xstr(operand1) + "\t\t" + xstr(operand2) + "\t\t" + \
                          xstr(operand3) + "\t\t" + xstr(label2) + "\n")
        i+=1

    binary.close()
    listing.close()

        
def convert_to_binary(value):
    bin_value = ""
    if re.compile("^0x([0-9a-fA-F]{1,2})$").match(value) != None:
          bin_value = bin(int(re.compile("^0x([0-9a-fA-F]{1,2})$").match(value).group(1), 16))[2:]
    elif re.compile("^0b([01]{1,8})$").match(value) != None:
          bin_value = re.compile("^0b([01]{1,8})$").match(value).group(1)
    elif re.compile("^(-[01]?[0-9]?[0-9])$").match(value) != None:
          bin_value = bin(2**9 + int(re.compile("^(-[01]?[0-9]?[0-9])$").match(value).group(1)))[3:]
    elif re.compile("^([+]?[01]?[0-9]?[0-9])$").match(value) != None:
          bin_value = bin(int(re.compile("^([+]?[01]?[0-9]?[0-9])$").match(value).group(1)))[2:]
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
        print "\tInvalid file!\n\tShould be with .ma extension"
        return None
    global SOURCE_FILE, FILENAME
    SOURCE_FILE = SOURCE_REGEX.group(0)
    FILENAME = SOURCE_REGEX.group(1)
    return SOURCE_FILE

def xstr(s):
    if s == None:
        return ""
    else:
        return s

asm_tool()

