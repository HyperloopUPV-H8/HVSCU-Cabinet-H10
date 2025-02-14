import json 
import Packet_descriptions as Pd
import os

packet_name= "        StackPacket* %name%;\n"
packet_struct = "   %name% = new StackPacket(%packet_data%);\n   packets[id] = %name%;\n   id++;"

def Generate_PacketDescription():
    with open("Core/Inc/Communications/JSON_ADE/boards.json") as f:
        boards = json.load(f)

    for board in boards["boards"]:
        with open("Core/Inc/Communications/JSON_ADE/" + (boards["boards"][board])) as f:
            b = json.load(f)
        board_instance = Pd.BoardDescription(board, b)
        globals()[board] = board_instance

def GenerateDataEnum(board:Pd.BoardDescription):
    Enums = set()
    for packet in board.packets:
        if packet != "orders":
            for packet_instance in board.packets[packet]:
                for measurement in packet_instance.measurements:
                    if hasattr(measurement, "enum"):
                        Enums.add(measurement.enum)
    Enums = list(Enums)
    enums_data = "\n".join(Enums)
    return enums_data
    
def GenerateData(board:Pd.BoardDescription):
    Data =set()
    for packet in board.packets:
        if packet != "orders":
            for packet_instance in board.packets[packet]:
                data = ""
                i=0
                data += "uint16_t &idpacket" + str(packet_instance.id) + ","
                for variable in packet_instance.variables:
                    data += (str(packet_instance.measurements[i].type)+" &"+ str(variable) +",")
                    i += 1  
                Data.add(data)
    Data = list(Data)
    if Data and Data[-1].endswith(","):
        Data[-1] = Data[-1][:-1]
    total_data ="".join(Data)
    return total_data
    
def GenerateDataNames(board:Pd.BoardDescription,packet_name:str):
    Names =[]
    for packet in board.packets:
        if packet != "orders":
            for packet_instance in board.packets[packet]:
                data = ""
                data += packet_name.replace("%name%",packet_instance.name)
                Names.append(data)
    names_data = "".join(Names)
    return names_data
    
    
def GenerateDataPackets(board:Pd.BoardDescription,packet_struct:str):
    Packets =[]
    for packet in board.packets:
        if packet != "orders":
            for packet_instance in board.packets[packet]:
                data = ""
                data +="idpacket"+str(packet_instance.id)+"," 
                
                for variable in packet_instance.variables:
                    data += (str(variable) +",")
                if data.endswith(","):
                        data = data[:-1]  
                aux = packet_struct
                aux = aux.replace("%name%",packet_instance.name)    
                Packets.append(aux.replace("%packet_data%", data))
                
    packets_data = "\n".join(Packets)
    return packets_data
    
def Generate_DataPackets_hpp(board_input:str):
    board_instance = globals()[board_input]
    
    if board_instance.data_size == 0:
        if os.path.exists("Core/Inc/Communications/Packets/DataPackets.hpp"):
            os.remove("Core/Inc/Communications/Packets/DataPackets.hpp")
        return
    
    with open("Core/Inc/Communications/Packets/Packet_generation/DataTemplate.hpp","r") as Input:
        data= Input.read()

    data = data.replace("%board%", board_instance.name)
    data = data.replace("%enums%", GenerateDataEnum(board_instance))
    data = data.replace("%packetnames%", GenerateDataNames(board_instance,packet_name))
    data = data.replace("%size%", str(board_instance.data_size))
    data = data.replace("%data%", GenerateData(board_instance))
    data = data.replace("%packets%", GenerateDataPackets(board_instance,packet_struct))

    with open("Core/Inc/Communications/Packets/DataPackets.hpp","w") as Output:
        Output.write(data)
            
def GenerateOrderEnum(board:Pd.BoardDescription):
    Enums = set()
    for packet in board.packets:
        if packet == "orders":
            for packet_instance in board.packets[packet]:
                for measurement in packet_instance.measurements:
                    if hasattr(measurement, "enum"):
                        Enums.add(measurement.enum)
    Enums = list(Enums)
    enums_data = "\n".join(Enums)
    return enums_data

def GenerateOrderData(board:Pd.BoardDescription):
    Data =set()
    for packet in board.packets:
        if packet == "orders":
            for packet_instance in board.packets[packet]:
                data = ""
                i=0
                data += "uint16_t &idpacket" + str(packet_instance.id) + ","
                for variable in packet_instance.variables:
                    data += (str(packet_instance.measurements[i].type)+" &"+ str(variable) +",")
                    i += 1  
                Data.add(data)
    Data = list(Data)
    if Data and Data[-1].endswith(","):
        Data[-1] = Data[-1][:-1]
    total_data ="".join(Data)
    return total_data

def GenerateOrderNames(board:Pd.BoardDescription,packet_name:str):
    Names =[]
    for packet in board.packets:
        if packet == "orders":
            for packet_instance in board.packets[packet]:
                data = ""
                data += packet_name.replace("%name%",packet_instance.name)
                Names.append(data)
    names_data = "".join(Names)
    return names_data

def GenerateOrderPackets(board:Pd.BoardDescription,packet_struct:str):
    Packets =[]
    for packet in board.packets:
        if packet == "orders":
            for packet_instance in board.packets[packet]:
                data = ""
                data +="idpacket"+str(packet_instance.id)+"," 
                
                for variable in packet_instance.variables:
                    data += (str(variable) +",")
                if data.endswith(","):
                        data = data[:-1]  
                aux = packet_struct
                aux = aux.replace("%name%",packet_instance.name)    
                Packets.append(aux.replace("%packet_data%", data))
                
    packets_data = "\n".join(Packets)
    return packets_data

def Generate_OrderPackets_hpp(board_input:str):
    board_instance = globals()[board_input]
    
    if board_instance.order_size == 0:
        if os.path.exists("Core/Inc/Communications/Packets/OrderPackets.hpp"):
            os.remove("Core/Inc/Communications/Packets/OrderPackets.hpp")
        return    
  
    with open("Core/Inc/Communications/Packets/Packet_generation/OrderTemplate.hpp","r") as Input:
        data= Input.read()

    data = data.replace("%board%", board_instance.name)
    data = data.replace("%enums%", GenerateOrderEnum(board_instance))
    data = data.replace("%packetnames%", GenerateOrderNames(board_instance,packet_name))
    data = data.replace("%size%", str(board_instance.order_size))
    data = data.replace("%data%", GenerateOrderData(board_instance))
    data = data.replace("%packets%", GenerateOrderPackets(board_instance,packet_struct))

    with open("Core/Inc/Communications/Packets/OrderPackets.hpp","w") as Output:
        Output.write(data)
    


Generate_PacketDescription()
board = input("Enter board name: ")
while board not in ["VCU","OBCCU","LCU"]:
    print("Board not found, only VCU, OBCCU and LCU are available")
    board = input("Enter board name: ")
Generate_DataPackets_hpp(board)
Generate_OrderPackets_hpp(board)







        