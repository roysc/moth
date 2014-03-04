
class Location():
    default = "0"
    def __init__(self, name = default):
        self._name = name
    def __eq__(self, o):
        return self._name == o._name
        
        
class AgentState():
    def __init__(self, loc):
        self._loc = loc
        

class Gender(Enum):
    male, female = range(2)

class Agent():
    #
    def __init__(self,
                 **params
             ):
        self._age = (0, 0)
        self._height = params["height"]
        self._gender = params["gender"]
        self._race = params["race"]

        self._state = AgentState()
        
# (LS, Q) -> LS
def run_choice(astate, choice):
    
    
    

class Player():
    
    pass
    



    
def test():
        
    a = Agent({
        height = 1,
        gender = Gender.male,
        race = [0]
    })


