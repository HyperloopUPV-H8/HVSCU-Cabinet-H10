import re
import json
enum_template = "enum class %name%{ \n %values% \n};"


class BoardDescription:    
    def __init__(self,name:str,board:dict):
        self.name = name
        self.id = board["board_id"]
        self.ip = board["board_ip"]
        self.data_size =0
        self.order_size =0
        i = 0
        self.packets = {}
        for packets in board["packets"]:
            packets_name = re.split(r'_|\.', packets)[0]  
            self.packets[packets_name] = []
            measurement = self._MeasurementFileSearch(packets,board["measurements"])
            with open("Core/Inc/Communications/Packets/Packet_generation/JSON_ADE/boards/" + name+"/" + board["packets"][i]) as f:
                p= json.load(f)
            with open("Core/Inc/Communications/Packets/Packet_generation/JSON_ADE/boards/" + name + "/" + measurement) as f:
                m = json.load(f)
            i += 1
            for packet in p["packets"]:
                self.packets[packets_name].append(PacketDescription(packet,m))
                if packets_name != "orders":
                    self.data_size += 1
                else:
                    self.order_size += 1
    @staticmethod            
    def _MeasurementFileSearch(packet:str,measurements:dict):
        packet_name = packet.split('_')[0]
        for measurement in measurements:
            measurement_name = measurement.split('_')[0]
            if packet_name[0] == measurement_name[0]:
                return measurement
        else:
            return measurements[0]
class PacketDescription:
    def __init__(self, packet:dict,measurements:dict):
        self.id =packet["id"]
        self.name = (packet["name"].replace(" ", "_"))
        self.type = packet["type"]
        self.variables = []
        self.measurements = []
        i=0
        for variable in packet["variables"]:
            self.variables.append(variable["name"])
            self.measurements.append(MeasurmentsDescription(measurements,variable["name"]))


class MeasurmentsDescription:
    def __init__(self,measurements:dict, variable:str):
        self.id = variable
        measurement = self._MeasurementSearch(measurements,variable)
        if measurement is None:
            raise Exception("Measurement not found")
        else:
            self.name = measurement["name"]
            self.type = self._unsigned_int_correction(measurement["type"])
            if self.type == "enum":
                self.enum = self._create_enum(measurement)
                self.type = measurement["id"]
                
    @staticmethod
    def _MeasurementSearch(measurements:dict, variable:str):
        for measurment in measurements["measurements"]:
            if measurment["id"] == variable:
                return measurment
        return None
    
    @staticmethod
    def _create_enum(measurement: dict):
        if "enumValues" not in measurement:
            raise ValueError("Measurement does not contain 'enumValues'")

        enum = enum_template.replace("%name%", measurement["id"])
        values = ""
        for value in measurement["enumValues"]:
            values += value + ",\n"
        if values.endswith(",\n"):
            values = values[:-2]
            values += "\n"
        enum = enum.replace("%values%", values.strip())
        return enum
    
    @staticmethod
    def _unsigned_int_correction(type:str):
        aux_type = type[:4]
        if aux_type == "uint":
            type += "_t"
        return type
        