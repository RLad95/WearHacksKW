//
//  ViewController.swift
//  WearHacks
//
//  Created by Salman Jalali on 2016-03-18.
//  Copyright © 2016 Salman Jalali. All rights reserved.
//

import Foundation
import UIKit
import CoreLocation
import MapKit

class ViewController: UIViewController,CLLocationManagerDelegate, MKMapViewDelegate, UITextFieldDelegate {

    @IBOutlet weak var slider1: UISlider!
    @IBOutlet weak var slider2: UISlider!
    @IBOutlet weak var slider3: UISlider!
    @IBOutlet weak var slider4: UISlider!
    
    var sentAlert: UIAlertController?
    
    @IBOutlet weak var color1: UILabel!
    @IBOutlet weak var color2: UILabel!
    @IBOutlet weak var color3: UILabel!
    var color1Value: CGFloat = 0.1
    var color2Value: CGFloat = 0.0
    var color3Value: CGFloat = 0.0
    
    func sendMessage(){
        let headers = [
            "authorization": "Basic QUNlOTkxN2Q2ZGQxMmIzMDIyODI1MjFiZjAxNWQ0YjMxYTplNTVlY2VkZDQ3NzQ0NmViZjliZWNjZTM4MDQ5MGNjOQ==",
            "cache-control": "no-cache",
            "postman-token": "cb5239ce-8434-bfb6-2fde-9794254ad131",
            "content-type": "application/x-www-form-urlencoded"
        ]
        
        var postData = NSMutableData(data: "To=+15195029071".dataUsingEncoding(NSUTF8StringEncoding)!)
        postData.appendData("&Body=Sal will arrive to your location shortly.".dataUsingEncoding(NSUTF8StringEncoding)!)
        postData.appendData("&From=+12268871365".dataUsingEncoding(NSUTF8StringEncoding)!)
        
        var request = NSMutableURLRequest(URL: NSURL(string: "https://api.twilio.com/2010-04-01/Accounts/ACe9917d6dd12b302282521bf015d4b31a/Messages")!,
            cachePolicy: .UseProtocolCachePolicy,
            timeoutInterval: 10.0)
        request.HTTPMethod = "POST"
        request.allHTTPHeaderFields = headers
        request.HTTPBody = postData
        
        let session = NSURLSession.sharedSession()
        let dataTask = session.dataTaskWithRequest(request, completionHandler: { (data, response, error) -> Void in
            if (error != nil) {
                print(error)
            } else {
                let httpResponse = response as? NSHTTPURLResponse
                print(httpResponse)
            }
        })
        
        dataTask.resume()
    }
    
    @IBAction func sendMessage(sender: AnyObject) {
        sendMessage()
        sentAlert = UIAlertController(title: "Message Sent", message:"The message was sent.", preferredStyle: UIAlertControllerStyle.Alert)
        sentAlert!.addAction(UIAlertAction(title: "Ok", style: UIAlertActionStyle.Default){
            (action:UIAlertAction)-> Void in
            //Do nothing
            }
        )
        presentViewController(sentAlert!, animated: true, completion: nil)
    }
    
    @IBOutlet weak var phone: UITextField!
    
    var long: Float = 0.0
    var lat : Float = 0.0
    var brightness: Int = 256
    
    var locationManager: CLLocationManager!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        locationManager = CLLocationManager()
        locationManager.desiredAccuracy = kCLLocationAccuracyBest;
        locationManager.delegate = self;
        // Do any additional setup after loading the view, typically from a nib.
        // user activated automatic authorization info mode
        let status = CLLocationManager.authorizationStatus()
        if status == .NotDetermined || status == .Denied || status == .AuthorizedWhenInUse {
            // present an alert indicating location authorization required
            // and offer to take the user to Settings for the app via
            // UIApplication -openUrl: and UIApplicationOpenSettingsURLString
            locationManager.requestAlwaysAuthorization()
            locationManager.requestWhenInUseAuthorization()
        }
        locationManager.startUpdatingLocation()
        locationManager.startUpdatingHeading()
        
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func sliderValueChanged(sender: UISlider) {
        let currentValue = Int(sender.value)
        color1Value = CGFloat(currentValue)/255.0
        color1.text = "\(currentValue)"
        print(color1Value)
        self.view.backgroundColor = UIColor(red: color1Value, green: color2Value, blue: color3Value, alpha: 1.0)
    }

    @IBAction func slider2ValueChanged(sender: UISlider) {
        let currentValue2 = Int(sender.value)
        color2Value = CGFloat(currentValue2)/255.0
        color2.text = "\(currentValue2)"
        print(color2Value)
        self.view.backgroundColor = UIColor(red: color1Value, green: color2Value, blue: color3Value, alpha: 1.0)
    }
    
    @IBAction func slider3ValueChanged(sender: UISlider) {
        let currentValue3 = Int(sender.value)
        color3Value = CGFloat(currentValue3)/255.0
        color3.text = "\(currentValue3)"
        print(color3Value)
        self.view.backgroundColor = UIColor(red: color1Value, green: color2Value, blue: color3Value, alpha: 1.0)
    }
    
    @IBAction func slider4ValueChanged(sender: UISlider) {
        let currentValue4 = Int(sender.value)
        brightness = currentValue4
        print("Brighness: \(brightness)")
    }
    
    @IBAction func redColor(sender: AnyObject) {
        color1.text = "256"
        color2.text = "0"
        color3.text = "0"
        self.view.backgroundColor = UIColor(red: 1.0, green: 0.0, blue: 0.0, alpha: 1.0)
    }
    
    @IBAction func greenColor(sender: AnyObject) {
        color1.text = "0"
        color2.text = "256"
        color3.text = "0"
        self.view.backgroundColor = UIColor(red: 0.0, green: 1.0, blue: 0.0, alpha: 1.0)
    }
    
    @IBAction func yellowColor(sender: AnyObject) {
        color1.text = "255"
        color2.text = "216"
        color3.text = "0"
        self.view.backgroundColor = UIColor(red: 255/255.0, green: 216/255.0, blue: 0.0, alpha: 1.0)
    }
    
    @IBAction func sendPreset(sender: AnyObject) {
        print("trying to send preset")
        loadResponse()
    }

    func loadResponse(){
        var string = "http://192.168.254.42:3000/frogger?" + "color1=" + color1.text!
        string += "&color2="+color2.text! + "&color3=" + color3.text!
        string += "&brightness=" + String(brightness)
        let url = NSURL(string: string)
        
        let session = NSURLSession.sharedSession()

        let request = NSMutableURLRequest(URL: url!)
        request.HTTPMethod = "GET";
        request.cachePolicy = NSURLRequestCachePolicy.ReloadIgnoringCacheData;
        
        let task = session.dataTaskWithRequest(request) {
            (
            let data, let response, let error) in
            guard let _:NSData = data,  let _:NSURLResponse = response where error == nil else {
                print("error");
                return;
            }
            let dataString = NSString(data :data!, encoding: NSUTF8StringEncoding);
            print(dataString);
            do {
                // Decode the JSON response.
                let json: AnyObject? = try NSJSONSerialization.JSONObjectWithData(data!, options: NSJSONReadingOptions.MutableContainers);
                print(json)
            }
            catch {
                print(error);
            }
        };
        task.resume();
    }
    
    
    func locationManager(manager: CLLocationManager, didUpdateToLocation newLocation: CLLocation, fromLocation oldLocation: CLLocation) {
            print("present location : \(newLocation.coordinate.latitude), \(newLocation.coordinate.longitude)")
    }
    
    override func prepareForSegue(segue: UIStoryboardSegue, sender: AnyObject?) {
        if segue.identifier == "seeLocation" {
            
        }
    }
}

