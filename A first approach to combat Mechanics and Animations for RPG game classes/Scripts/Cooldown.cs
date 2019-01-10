using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Cooldown : MonoBehaviour {

	public int time = 100;
	public Slider cooldown;


	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		//updating of cooldown bar
		cooldown.value = time;

		if(time < 100){
		//call of reset method after 10 seconds
		Invoke ("Reset",10);

		}

		
	}

	void Reset(){
		//reset value of variable time to 100
		time = 100;


	}

}
