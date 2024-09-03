from doodle import *

class Sandbox(Application):
    @staticmethod
    def Create():
        DOO_CORE_INFO("Hello")
        return Sandbox()

    def Initialize(self) -> None:
        super().Initialize()
        DOO_CORE_INFO("Initialize")
    
    def Deinitialize(self) -> None:
        super().Deinitialize()
        DOO_CORE_INFO("Deinitialize")

    def BeforeLayout(self) -> None:
        super().BeforeLayout()
        DOO_CORE_INFO("BeforeLayout")
        
    def AfterLayout(self) -> None:
        super().AfterLayout()
        DOO_CORE_INFO("AfterLayout")
    
    def BeforeUpdate(self) -> None:
        super().BeforeUpdate()
        DOO_CORE_INFO("BeforeUpdate")
    
    def AfterUpdate(self) -> None:
        super().AfterUpdate()
        DOO_CORE_INFO("AfterUpdate")
    
    def BeforeRender(self) -> None:
        super().BeforeRender()
        DOO_CORE_INFO("BeforeRender")
    
    def AfterRender(self) -> None:
        super().AfterRender()
        DOO_CORE_INFO("AfterRender")

def main():
    Log.Initialize()
    runner = ApplicationRunner.Get()
    props = WindowProps("Doodle", 1920, 1080)
    runner.CreateApp(Sandbox, props)
    runner.Run()

if __name__ == "__main__":
    main()