using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class Results : MonoBehaviour
{
    public Text text1;
    public Text text2;

    // Start is called before the first frame update
    void Start()
    {
        text1.text = "Time :" + ActorScript.timer;
        text2.text = "Times died:" + ActorScript.death_counter;
    }

    
}
