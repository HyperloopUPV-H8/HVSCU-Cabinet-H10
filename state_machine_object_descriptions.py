class State:
    def __init__(self, name:str):
        self.name=name

class Transition:
    def __init__(self, name:str,from_state:str,to_state:str,description:str=None):
        self.name=name
        self.from_state=from_state
        self.to_state=to_state
        self.comment=description

class Enter:
    def __init__(self, state: str, state_actions: list):
        self.state = state
        self.state_actions = state_actions

class CyclicLowPrecision:
    def __init__(self, states: list, state_actions: list):
        self.states = states
        self.state_actions = state_actions

class CyclicMidPrecision:
    def __init__(self, states: list, state_actions: list):
        self.states = states
        self.state_actions = state_actions

class CyclicHighPrecision:
    def __init__(self, states: list, state_actions: list):
        self.states = states
        self.state_actions = state_actions

class Exit:
    def __init__(self, state: str, state_actions: list):
        self.state = state
        self.state_actions = state_actions

class StateReference:
    def __init__(self, state_data):
        if isinstance(state_data, str):
            self.state_name = state_data
            self.nested_machine = None
            self.sub_state = None
        else:
            self.state_name = None
            self.nested_machine = state_data["name"]
            self.sub_state = state_data["sub-state"]

    def __str__(self):
        if self.nested_machine:
            return f"{self.nested_machine}.{self.sub_state}"
        return self.state_name

class ActionDefinition:
    def __init__(self, action_data):
        if isinstance(action_data, str):
            self.name = action_data
            self.description = None
            self.period = None
        else:
            self.name = action_data.get("name") or action_data.get("action_name")
            self.description = action_data.get("description")
            self.period = action_data.get("action_period")

class Action:
    def __init__(self):
        self.type = None
        self.state = None
        self.state_actions = []

    @classmethod
    def create_enter_exit(cls, action_type, state_data, actions_data):
        action = cls()
        action.type = action_type
        action.state = StateReference(state_data)
        action.state_actions = [ActionDefinition(a) for a in actions_data]
        return action

    @classmethod
    def create_cyclic(cls, precision, states_data, actions_data):
        actions = []
        for state in states_data:
            for action_data in actions_data:
                action = cls()
                action.type = f"cyclic_{precision}"
                action.state = StateReference(state)
                action.state_actions = [ActionDefinition(action_data)]
                actions.append(action)
        return actions

    def describe(self):
        states_str = ", ".join(str(s) for s in self.states)
        actions_str = ", ".join(
            f"{a.name}" + (f"({a.period})" if a.period else "") + 
            (f" - {a.description}" if a.description else "")
            for a in self.actions
        )
        return f"{self.type} action on {states_str}: {actions_str}"

class StateMachine:
    def __init__(self,name:str):
        self.name=name
        self.states=[]
        self.transitions=[]
        self.actions=[]
        self.nested_state_machines=[]
    
    def add_state(self,state:State):
        self.states.append(state)

    def add_transition(self,transition:Transition):
        self.transitions.append(transition)

    def add_action(self, action:Action):
        self.actions.append(action)
    
    def add_nested_state_machine(self, state_machine:'StateMachine'):
        self.nested_state_machines.append(state_machine)


def parse_state(data):
    if isinstance(data, str):
        return State(data)
    elif isinstance(data, dict):
        nested_sm = StateMachine(data["name"])
        nested_sm.nested_to = data.get("nested_to")
        for sub_state in data["sub-states"]:
            nested_sm.add_state(State(sub_state))
        return nested_sm

def parse_transition(data):
    name = data["transition_name"]
    description = data.get("description")

    from_state = StateReference(data["from_state"])
    to_state = StateReference(data["to_state"]) 
    
    return Transition(name, from_state, to_state, description)

def parse_actions(data):
    actions_list = []
    
    actions_data = data.get("actions", {})
    
    for enter_data in actions_data.get("enter", []):
        action = Action.create_enter_exit(
            "enter",
            enter_data["state"],
            enter_data["state_actions"]
        )
        actions_list.append(action)

    cyclic_data = actions_data.get("cyclic", {})
    for precision in ["low_precision", "mid_precision", "high_precision"]:
        for cyclic_item in cyclic_data.get(precision, []):
            actions = Action.create_cyclic(
                precision,
                cyclic_item["states"],
                cyclic_item["state_actions"]
            )
            actions_list.extend(actions)

    for exit_data in actions_data.get("exit", []):
        action = Action.create_enter_exit(
            "exit",
            exit_data["state"],
            exit_data["state_actions"]
        )
        actions_list.append(action)

    return actions_list

def parse_state_machine(data):
    sm = StateMachine(data["name"])

    for st_data in data["states"]:
        parsed = parse_state(st_data)
        if isinstance(parsed, State):
            sm.add_state(parsed)
        else:
            sm.add_nested_state_machine(parsed)

    for tr_data in data["transitions"]:
        tr = parse_transition(tr_data)
        sm.add_transition(tr)

    parsed_actions = parse_actions(data)
    sm.actions = parsed_actions

    return sm

def get_state_reference(state_ref, state_machine_name):
    if isinstance(state_ref, str):
        return f"{state_machine_name}States::{state_ref.upper()}"
    
    if state_ref.nested_machine:
        return f"{state_ref.nested_machine}::{state_ref.sub_state.upper()}"
    return f"{state_machine_name}States::{state_ref.state_name.upper()}"