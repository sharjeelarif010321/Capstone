//
//  UserViewModel.swift
//  SafeSnooze
//
//  Created by Sharjeel Arif on 2023-03-31.
//

import Foundation
import FirebaseFirestore

import Foundation
import FirebaseDatabase

class userViewModel: ObservableObject {
    
    @Published var users = [User]()
    private var db = Firestore.firestore()
    
    func fetchData() {
        let ref = Database.database().reference()
        ref.observe(.value) { (snapshot) in
            if let dict = snapshot.value as? [String: AnyObject] {
                self.users.removeAll()
                for (_, value) in dict {
                    if let data = value as? [String: AnyObject] {
                        let heartrate = (data["Heartrate"] as? [String: Any])?["BPM"] as? String ?? ""
                        let position = (data["Position"] as? [String: Any])?["Value"] as? String ?? ""
                        let posture = (data["Posture"] as? [String: Any])?["Subject is"] as? String ?? ""
                        let spo2 = (data["SpO2"] as? [String: Any])?["Percentage"] as? String ?? ""
                        let status = (data["Status"] as? [String: Any])?["Subject is"] as? String ?? ""
                        
                        let user = User(heartrate: heartrate, position: position, posture: posture, spo2: spo2, status: status)
                        self.users.append(user)
                    }
                }
            }
        }
    }
}
