using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Player_combat : MonoBehaviour {

	Animator anim;
	public static Transform opponent;
	public float tapSpeed = 0.2f;
	private float lastTapTime = -10f;
	private int clickCount = 0;
	bool attack,attack2,attack3;
	int health = 100;
	public Slider healthbar;



	void OnTriggerEnter(Collider other){
	

		//lose health only if player collides with the tag"En_Weapon"
		if(other.tag == "En_Weapon"){
		health -= 20;
		
		}
		//if health less than 0,player dies
		if (health <= 0) {

			anim.SetBool ("Death",true);

		}

	
	}



	void Awake () {

		anim = GetComponent<Animator>();

	}

	// Update is called once per frame
	void Update () 
	{
		//if the variable for cooldown is above 0,the player can attack
		if(gameObject.GetComponent<Cooldown> ().time > 0){
		Attack ();
		}
		//update of player's health bar
		healthbar.value = health;
	

	
	}


	void Attack(){
	
		//attack regarding the times that player pressed left mouse button
		if ((Input.GetMouseButtonDown (0) )  ) 
		{
			anim.SetTrigger ("attack1");
			lastTapTime = Time.time;

			if ((Time.time - lastTapTime) < tapSpeed)
			{
				clickCount += 1;

				if(clickCount >= 3)
				{
					attack = true;
					attack2 = true;
					attack3 = true;

					anim.SetBool("attack",attack);
					anim.SetBool("attack2",attack2);
					anim.SetBool("attack3",attack3);
					gameObject.GetComponent<Cooldown> ().time -= 60;
					Debug.Log ("three click");
					clickCount = 0;



				}
			}
		}

		if ((Time.time - lastTapTime) > tapSpeed)
		{
			if (clickCount == 2)
			{
				attack = true;
				attack2 = true;

				anim.SetBool("attack",attack);
				anim.SetBool("attack2",attack2);
				gameObject.GetComponent<Cooldown> ().time -= 40;
			
				Debug.Log ("double click");
				clickCount = 0;
			

			}
			else if (clickCount == 1)
			{
				attack = true;
			
				anim.SetBool("attack",attack);
				gameObject.GetComponent<Cooldown> ().time -= 20;
			
				Debug.Log ("one click");
				clickCount = 0;
			

			}

		

			clickCount = 0;
		}
	
	
	}









}
