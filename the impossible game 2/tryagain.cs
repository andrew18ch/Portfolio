using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;

public class tryagain : MonoBehaviour
{
   public void try_again() {

        SceneManager.LoadScene("SampleScene");

    }

    public void exit()
    {

        SceneManager.LoadScene("Menu");

    }

    public void next() {

        SceneManager.LoadScene("Instructions");

    }

}
