using UnityEngine;

public class Time_Controller : MonoBehaviour {

	public float slowdownFactor = 0.05f;
	public float slowdownLength = 2f;

	void Update(){

		Time.timeScale += (1f / slowdownLength) * Time.unscaledTime ;
		Time.timeScale = Mathf.Clamp (Time.timeScale,0f,1f);

	}



	public void Slowmotion(){

		Time.timeScale = slowdownFactor;
		Time.fixedDeltaTime = Time.timeScale * .02f;

	}



}
