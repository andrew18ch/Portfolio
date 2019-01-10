using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Shield : MonoBehaviour {

	void OnTriggerEnter(Collider other){

	    //reduce amount of cooldown variable if the player uses the shield
		gameObject.GetComponent<Cooldown> ().time -= 10;

		}



	// Use this for initialization
	void Start () {
		
	}
	
	// Update is called once per frame
	void Update () {
		
	}
}
