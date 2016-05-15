//
//  SetTripViewController.swift
//  WearHacks
//
//  Created by Salman Jalali on 2016-03-19.
//  Copyright © 2016 Salman Jalali. All rights reserved.
//

import Foundation
import UIKit
import MapKit
import CoreLocation

class SetTripController: UIViewController, UITextFieldDelegate, CLLocationManagerDelegate {
    var lat: Float = 43.477572
    var long: Float = -80.549226
    var guestLat: Float = 0.0
    var guestLong: Float = 0.0
    var sentAlert: UIAlertController?
    var distance: Double = 0.0
    
    let baseUrl = "https://maps.googleapis.com/maps/api/geocode/json?"
    let apikey = "AIzaSyAg_boGied9KXRgtI_1xrPM33ZzOY21zyo"
    
    @IBOutlet weak var zipCode: UITextField!
    @IBOutlet weak var name: UITextField!
    @IBOutlet weak var phone: UITextField!
    
    @IBAction func sendMessage(sender: AnyObject) {
        sentAlert = UIAlertController(title: "Send Message", message:"We will send the phone number a message once you are within the vicinity.", preferredStyle: UIAlertControllerStyle.Alert)
        sentAlert!.addAction(UIAlertAction(title: "Ok", style: UIAlertActionStyle.Default){
            (action:UIAlertAction)-> Void in
                self.findAndSetup()
            }
        )
        presentViewController(sentAlert!, animated: true, completion: nil)
    }
    
    func findAndSetup(){
        getLatLngForZip(zipCode.text!)
        sendMessage()
    }
    
    func getLatLngForZip(zipCode: String) {
        let url = NSURL(string: "\(baseUrl)address=\(zipCode)&key=\(apikey)")
        let data = NSData(contentsOfURL: url!)
        let json = try! NSJSONSerialization.JSONObjectWithData(data!, options: NSJSONReadingOptions.AllowFragments) as! NSDictionary
        if let result = json["results"] as? NSArray {
            if let geometry = result[0]["geometry"] as? NSDictionary {
                if let location = geometry["location"] as? NSDictionary {
                    guestLat = location["lat"] as! Float
                    guestLong = location["lng"] as! Float
                    print("\n\(guestLat), \(guestLong)")
                    let userLocation:CLLocation = CLLocation(latitude: Double(self.lat), longitude: Double(self.long))
                    let priceLocation:CLLocation = CLLocation(latitude: Double(guestLat), longitude: Double(guestLong))
                    let meters:CLLocationDistance = userLocation.distanceFromLocation(priceLocation)
                    self.distance = meters
                    print("Distance: \(meters)")
                }
            }
        }
    }
    
    func sendMessage(){
        let headers = [
            "authorization": "Basic QUNlOTkxN2Q2ZGQxMmIzMDIyODI1MjFiZjAxNWQ0YjMxYTplNTVlY2VkZDQ3NzQ0NmViZjliZWNjZTM4MDQ5MGNjOQ==",
            "cache-control": "no-cache",
            "postman-token": "cb5239ce-8434-bfb6-2fde-9794254ad131",
            "content-type": "application/x-www-form-urlencoded"
        ]
        
        let postData = NSMutableData(data: "To=\(phone!.text!)".dataUsingEncoding(NSUTF8StringEncoding)!)
        postData.appendData("&Body=\(name!.text!) is \(distance/1000.0) Km away from you and will arrive to your location (\(zipCode!.text!)) shortly.".dataUsingEncoding(NSUTF8StringEncoding)!)
        postData.appendData("&From=+12268871365".dataUsingEncoding(NSUTF8StringEncoding)!)
        
        let request = NSMutableURLRequest(URL: NSURL(string: "https://api.twilio.com/2010-04-01/Accounts/ACe9917d6dd12b302282521bf015d4b31a/Messages")!,
            cachePolicy: .UseProtocolCachePolicy,
            timeoutInterval: 20.0)
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
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
    }
    
    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}
