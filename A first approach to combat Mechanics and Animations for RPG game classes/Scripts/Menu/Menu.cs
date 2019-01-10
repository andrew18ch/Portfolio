using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.SceneManagement;

public class Menu : MonoBehaviour {

	public void Warrior(){
		//load of the scene: test_scene
		SceneManager.LoadScene ("test_scene");
			
	}

	public void Mage(){
		//load of the scene: mage_scene
		SceneManager.LoadScene ("mage_scene");

	}

	public void Archer(){
		//load of the scene: archer_scene
		SceneManager.LoadScene ("archer_scene");

	}

	public void Button(){
		//load of the scene: buttons
		SceneManager.LoadScene ("buttons");

	}

}
