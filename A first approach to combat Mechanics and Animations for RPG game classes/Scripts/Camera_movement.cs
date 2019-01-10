using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Camera_movement : MonoBehaviour {

	public Transform player_camera, center_point;
	public float distance,max_height,min_height,orbiting_speed,vertical_speed;
	float height;
	Vector3 dest;
	RaycastHit camera_hit;

	void Update(){
	
		//position of player's head
		center_point.position = gameObject.transform.position + new Vector3(0 , 1.57f , 0);
		center_point.eulerAngles += new Vector3(0,Input.GetAxis("Mouse X") * Time.deltaTime * orbiting_speed,0) ; 
		height += Input.GetAxis ("Mouse Y") * Time.deltaTime * -vertical_speed ;
		height = Mathf.Clamp(height,min_height,max_height) ;

	}

	void FixedUpdate(){
	
		dest = center_point.position + center_point.forward * -1 * distance + Vector3.up * height  ;
		//camera collision with object
		if(Physics.Linecast(center_point.position,dest,out camera_hit)){
			
			if(camera_hit.collider.CompareTag("Building")){

				player_camera.position = camera_hit.point + camera_hit.normal * 0.14f ;

			}

		}

		player_camera.position = Vector3.Lerp (player_camera.position,dest,Time.deltaTime*10);
		player_camera.LookAt (center_point);
	
	
	}



}
