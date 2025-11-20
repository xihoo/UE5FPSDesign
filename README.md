# UE5FPSDesign
UE5 FPS Design

## 这是一个UE5 支持多人PVE 的 FPS 游戏项目的设计文档，请永远使用中文回答问题，编写文档等

编写文档时 注意：  
1. 尽量结合GAS框架实现
2. 尽量使用c++实现
3. 尽量使用数据驱动的方式
4. 注意这是一个支持多人的游戏
5. 注意射击框架参考SKG Shooter Framework框架的实现方式，但是不要直接使用这个插件，因为这个插件不直接集成GAS
6. 输入系统统一采用“InputAction -> GameplayTag(Input.*) -> GameplayAbility(Ability.*)” 的数据资产绑定模式，所有战斗/交互输入通过 ASC 激活能力并支持客户端预测。
