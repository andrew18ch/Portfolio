using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Character_movement : MonoBehaviour {

	Animator anim;
	public Transform center_point;
	public float rotation_speed;
	bool forward,left,right,back;
	bool backstep = false;
	bool slow_walk = true;
	int rotation_angle;

	void Awake(){

		anim = GetComponent<Animator>();

	}

	void Update(){

		//player movement with WASD buttons
		forward = Input.GetKey (KeyCode.W);
		back = Input.GetKey (KeyCode.S);
		left = Input.GetKey (KeyCode.A);
		right = Input.GetKey (KeyCode.D);

		//running
		if (Input.GetButtonDown ("Fire3")) {
			

			slow_walk = !slow_walk;

		} else if (Input.GetButtonUp ("Fire3")) {

			slow_walk = !slow_walk; 


		}

		anim.SetBool ("walk_slow",slow_walk);
		//backstep
		if (Input.GetKeyDown(KeyCode.B)) {

			backstep = true;
			Debug.Log (backstep);

		} else if (Input.GetKeyUp(KeyCode.B)) {

			backstep = false;
			Debug.Log (backstep);

		}

		anim.SetBool ("backstep",backstep);


		//movement regarding the angle
		Calculate_Angle ();
		anim.SetFloat ("movement",Mathf.Max( Mathf.Abs( Input.GetAxis("Vertical")),Mathf.Abs( Input.GetAxis("Horizontal"))));
		anim.SetFloat ("angle",Mathf.DeltaAngle(transform.eulerAngles.y,center_point.eulerAngles.y + rotation_angle));
		//περιστροφη με βαση την καμερα
		if(anim.GetCurrentAnimatorStateInfo(0).IsTag("turnable")){
			transform.eulerAngles += new Vector3 (0,Mathf.DeltaAngle(transform.eulerAngles.y,center_point.eulerAngles.y + rotation_angle) * Time.deltaTime * rotation_speed,0);
		}

	}

	//calculation of angle
	void Calculate_Angle(){


		if (forward && !back) {
			if (left && !right) {

				rotation_angle = -45;

			} else if (!left && right) {

				rotation_angle = 45;

			} else {

				rotation_angle = 0;

			}

		} else if (!forward && back) {
			if (left && !right) {

				rotation_angle = -135;

			} else if (!left && right) {

				rotation_angle = 135;

			} else {

				rotation_angle = 180;

			}
		} else {

			if (left && !right) {

				rotation_angle = -90;

			} else if (!left && right) {

				rotation_angle = 90;

			} else {

				rotation_angle = 0;

			}

		}

	}


}
