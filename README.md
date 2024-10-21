## 프로젝트 소개

3인칭 좀비 슈팅 게임 월드 워 z 모작 프로젝트

![image.jpg1](https://github.com/lsh210726/DBSSPJ2_WWZ/blob/main/readIMG/zombieClimb.png) |![image.jpg2](https://github.com/lsh210726/DBSSPJ2_WWZ/blob/main/readIMG/zomAttack.png)
--- | --- | 

https://www.youtube.com/watch?v=MxaGTrueEY0

개발인원 : 언리얼 3명  
개발기간 : 23/1/26 ~ 23/2/19 (3주)  


---
### 개발한 것 

- 좀비 길찾기, 공격
- 좀비 군집 움직임
- 좀비 생성 및 시체 제거
- 좀비 레그돌
- 좀비 애니메이션
  
---
### 사용기술
- 언리얼 엔진 5.3
- Unreal Blueprint
- C++

---
### 군집 움직임

![image.jpg1](https://github.com/lsh210726/DBSSPJ2_WWZ/blob/main/readIMG/zomrun.png) |![image.jpg2](https://github.com/lsh210726/DBSSPJ2_WWZ/blob/main/readIMG/zombieClimb.png)
--- | --- |

<img src="https://github.com/lsh210726/DBSSPJ2_WWZ/blob/main/readIMG/zonsworm.gif" alt="functionCallingIMG" style="width: 80%;"> 

언리얼의 기본 네비게이션 시스템을 사용할 시 모든 액터가 같은 경로로 줄지어 이동하기에 부자연스러운 움직임을 보입니다.


좀비가 떼로 몰려오는 움직임을 구현하기 위하여 RVO(Reciprocal Velocity Obstacles) 및 크라우드 우회(Detour Croud), 스폰 위치 변화를 활용한 자연스러운 군집 움직임을 구현하였습니다.


해당 옵션을 활용할 경우 좀비가 죽은 상태에서도 회피 움직임을 하는 문제가 발생하여 죽은 상태인 경우에는 군집 회피 설정을 끄는 방식으로 해결하였습니다.

---
### 오브젝트 풀링을 활용한 대규모 좀비 구현

많은 수의 좀비들이 레벨에 배치되어야 하기에 생성,삭제보다 메모리 관리 측면에서 유리한 오브젝트 풀링 방식으로 좀비 스폰을 구현하였습니다. 

더불어 죽은 좀비의 시체는 레벨에 레그돌로 남아있지만, 큐 방식의 시체 수집가 로직을 구현하여 레벨에는 일정한 수의 시체만이 배치되도록 하였습니다. 

죽은지 가장 오래된 레그돌부터 차례대로 제거되기에 플레이어는 어색함을 느끼지 못합니다.

<details>
<summary>코드</summary>

```c++
//좀비 만들기
for (int i = 0;  i < zomMaxNum;  i++)
{
  //좀비를 지정된 수만큼 만든다
  ALSH_BaseZom* zombie = GetWorld()->SpawnActor<ALSH_BaseZom>(zombieFactory);
  if (zombie == nullptr) continue;

  //좀비를 큐에 담는다
  zombieQueue.Add(zombie);
}

//시체 수집가
void ALSH_ZombieManager::bodyCollecting(class ALSH_BaseZom* zombie)
{
	//만약 좀비가 죽었는데 이미 레벨에 시체가 최대로 배치되어 있다면
	if (bodyMaxNum<=bodyList.Num())
	{
		//시체중 가장 먼저 죽은 좀비를 스폰 대기열에 등록한다
		zombieQueue.Add(bodyList[0]);
		//좀비를 초기화한다
		bodyList[0]->fsm->DeActiveAction();
		//시체리스트에서 제일 먼저 죽은 좀비를 뺀다
		bodyList.RemoveAt(0);
		//시체리스트에 새로 죽은 좀비를 등록한다
		bodyList.Add(zombie);

		//만약 스폰 대기열이 꽉 찼다면
		if (zombieQueue.Num() >= queueMaxNum)
		{
			//스폰모드를 켠다(한번에 대규모 스폰)
			bisSpawning = true;
		}
	}
  //레벨에 배치된 시체가 최대가 아니라면
	else
	{
    //죽은 좀비를 시체리스트에 등록한다
		bodyList.Add(zombie);
	}
}
```
</details>

---
### 좀비 벽타기

<img src="https://github.com/lsh210726/DBSSPJ2_WWZ/blob/main/readIMG/zomWallClimb.jpg" alt="functionCallingIMG" style="width: 60%;">

원작 게임에서 좀비들은 레벨의 특정 지점에서만 벽타기를 합니다. 

해당 기능을 구현하기 위해 우선 지상의 좀비들은 벽타기 모드에서는 오를 수 있는 표면으로 이동합니다. 

오를 수 있는 표면에 다다르면 벽을 타고 기어오릅니다. 

오르기가 끝나면 추적 모드가 되어 플레이어로 이동합니다.

---
### 좀비 상태 머신

좀비의 행동을 쉽게 관리하기 위해 유한 상태 머신을 사용했습니다.


- 좀비가 스폰되면 이동상태가 됩니다. 

- 이동상태

  플레이어를 향해 이동합니다. 공격 가능 거리에 플레이어가 있다면 공격상태로 전환합니다.

- 공격상태

  만약 플레이어가 공격 거리에 있다면 공격합니다. 플레이어가 공격 거리 밖이라면 이동상태로 전환합니다.

- 데미지상태

  데미지를 입었을 때 만약 좀비의 체력이 0보다 작다면 죽습니다.

 ```mermaid
%%{init: { 'theme': 'dark', 'themeVariables': { 'fontFamily': 'arial', 'textColor': 'white' } } }%%
stateDiagram-v2
    [*] --> Move: <b>스폰</b>
    
    Move --> Attack: <b>플레이어가 공격 범위 내</b>
    Attack --> Move: <b>플레이어가 공격 범위 밖</b>
    
    Move --> Damaged: <b>데미지 받음</b>
    Attack --> Damaged: <b>데미지 받음</b>
    
    Damaged --> Move: <b>체력 > 0</b>
    Damaged --> Death: <b>체력 <= 0</b>
    
    Death --> [*]

    state Move {
        [*] --> Walking
        Walking: <b>플레이어 방향으로 이동</b>
    }
    
    state Attack {
        [*] --> Attacking
        Attacking: <b>플레이어 공격</b>
    }
    
    state Damaged {
        [*] --> CheckHealth
        CheckHealth: <b>체력 확인</b>
    }
   ```

