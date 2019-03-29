using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Audiodie : MonoBehaviour
{
    public AudioClip clip;
    public AudioSource sour;

    // Start is called before the first frame update
    void Start()
    {
        sour.clip = clip;
        sour.Play();

    }
}
